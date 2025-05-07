#include <linux/module.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/debugfs.h>
#include "core.h"
static struct pinctrl_dev *global_pinctrl_dev;
static struct dentry *debugfs_root;

static const struct pinctrl_pin_desc pins[] = {
    PINCTRL_PIN(0, "pin0"),
    PINCTRL_PIN(1, "pin1"),
    PINCTRL_PIN(2, "pin2"),
    PINCTRL_PIN(3, "pin3")
};

struct virtual_functions_desc {
    const char *func_name;
    const char **groups;
    int num_groups;
};

static const char *fun0_groups[] = {"pin0", "pin1", "pin2", "pin3"};
static const char *fun1_groups[] = {"pin0", "pin1"};
static const char *fun2_groups[] = {"pin2", "pin3"};

static struct virtual_functions_desc global_functions_desc[] = {
    {"gpio", fun0_groups, 4},
    {"i2c",  fun1_groups, 2},
    {"uart", fun2_groups, 2},
};

static unsigned long global_configs[4];

// 自定义调试文件写入函数
static ssize_t pinctrl_configs_write(struct file *file, const char __user *buf,
                                     size_t count, loff_t *ppos)
{
    char kbuf[128];
    char cmd[32], dev_name[32], state_name[32], pin_name[32];
    unsigned long config;
    int pin_id = -1;
    int ret;

    if (count >= sizeof(kbuf)) {
        pr_err("Input too long: %zu bytes\n", count);
        return -EINVAL;
    }
    if (copy_from_user(kbuf, buf, count)) {
        pr_err("Failed to copy from user\n");
        return -EFAULT;
    }
    kbuf[count] = '\0';

    ret = sscanf(kbuf, "%31s %31s %31s %31s 0x%lx", cmd, dev_name, state_name, pin_name, &config);
    if (ret != 5 || strcmp(cmd, "modify") != 0 || strcmp(state_name, "default") != 0) {
        pr_err("Invalid command format: %s\n", kbuf);
        return -EINVAL;
    }

    for (int i = 0; i < ARRAY_SIZE(pins); i++) {
        if (strcmp(pin_name, pins[i].name) == 0) {
            pin_id = i;
            break;
        }
    }
    if (pin_id < 0) {
        pr_err("Invalid pin name: %s\n", pin_name);
        return -EINVAL;
    }

    global_configs[pin_id] = config;
    pr_info("Set %s config to 0x%lx\n", pin_name, config);

    return count;
}

// 自定义调试文件读取函数
static int pinctrl_configs_show(struct seq_file *s, void *unused)
{
    for (int i = 0; i < ARRAY_SIZE(pins); i++) {
        seq_printf(s, "pin %s: 0x%lx\n", pins[i].name, global_configs[i]);
    }
    return 0;
}

static int pinctrl_configs_open(struct inode *inode, struct file *file)
{
    return single_open(file, pinctrl_configs_show, NULL);
}

static const struct file_operations pinctrl_configs_fops = {
    .open = pinctrl_configs_open,
    .read = seq_read,
    .write = pinctrl_configs_write,
    .llseek = seq_lseek,
    .release = single_release,
};

static int virtual_get_groups_count(struct pinctrl_dev *pctldev)
{
    return pctldev->desc->npins;
}

static const char *virtual_get_group_name(struct pinctrl_dev *pctldev, unsigned selector)
{
    return pctldev->desc->pins[selector].name;
}

static int virtual_get_group_pins(struct pinctrl_dev *pctldev, unsigned selector,
                                 const unsigned int **pins, unsigned *num_pins)
{
    static unsigned int pin;
    if (selector >= pctldev->desc->npins)
        return -EINVAL;
    pin = pctldev->desc->pins[selector].number;
    *pins = &pin;
    *num_pins = 1;
    return 0;
}

static void virtual_pin_dbg_show(struct pinctrl_dev *pctldev, struct seq_file *s,
                                unsigned offset)
{
    seq_printf(s, "pin %s\n", dev_name(pctldev->dev));
}

static int virtual_dt_node_to_map(struct pinctrl_dev *pctldev, struct device_node *np,
                                 struct pinctrl_map **map, unsigned *nmaps)
{
    int num_pin = 0;
    const char *pin;
    const char *functions;
    unsigned int config;
    unsigned long *configs;

    while (of_property_read_string_index(np, "groups", num_pin, &pin) == 0)
        num_pin++;

    struct pinctrl_map *pctrl_map = kmalloc_array(num_pin * 2, sizeof(struct pinctrl_map), GFP_KERNEL);
    if (!pctrl_map)
        return -ENOMEM;

    for (size_t i = 0; i < num_pin; i++) {
        of_property_read_string_index(np, "groups", i, &pin);
        of_property_read_string_index(np, "functions", i, &functions);
        of_property_read_u32_index(np, "configs", i, &config);

        configs = kmalloc(sizeof(*configs), GFP_KERNEL);
        if (!configs) {
            kfree(pctrl_map);
            return -ENOMEM;
        }

        pctrl_map[i * 2].dev_name = dev_name(pctldev->dev);
        pctrl_map[i * 2].type = PIN_MAP_TYPE_MUX_GROUP;
        pctrl_map[i * 2].data.mux.function = functions;
        pctrl_map[i * 2].data.mux.group = pin;

        pctrl_map[i * 2 + 1].dev_name = dev_name(pctldev->dev);
        pctrl_map[i * 2 + 1].type = PIN_MAP_TYPE_CONFIGS_PIN;
        pctrl_map[i * 2 + 1].data.configs.group_or_pin = pin;
        pctrl_map[i * 2 + 1].data.configs.configs = configs;
        configs[0] = config;
        pctrl_map[i * 2 + 1].data.configs.num_configs = 1;
    }

    *map = pctrl_map;
    *nmaps = num_pin * 2;
    return 0;
}

static void virtual_dt_free_map(struct pinctrl_dev *pctldev, struct pinctrl_map *map,
                               unsigned num_maps)
{
    for (unsigned i = 0; i < num_maps; i++) {
        if (map[i].type == PIN_MAP_TYPE_CONFIGS_PIN)
            kfree(map[i].data.configs.configs);
    }
    kfree(map);
}

static const struct pinctrl_ops virtual_pctrl_ops = {
    .get_groups_count = virtual_get_groups_count,
    .get_group_name = virtual_get_group_name,
    .get_group_pins = virtual_get_group_pins,
    .pin_dbg_show = virtual_pin_dbg_show,
    .dt_node_to_map = virtual_dt_node_to_map,
    .dt_free_map = virtual_dt_free_map,
};

static int virtual_pmx_get_funcs_count(struct pinctrl_dev *pctldev)
{
    return ARRAY_SIZE(global_functions_desc);
}

static const char *virtual_pmx_get_func_name(struct pinctrl_dev *pctldev,
                                            unsigned selector)
{
    return global_functions_desc[selector].func_name;
}

static int virtual_pmx_get_groups(struct pinctrl_dev *pctldev, unsigned selector,
                                 const char * const **groups, unsigned *num_groups)
{
    *groups = global_functions_desc[selector].groups;
    *num_groups = global_functions_desc[selector].num_groups;
    return 0;
}

static int virtual_pmx_set(struct pinctrl_dev *pctldev, unsigned selector,
                          unsigned group)
{
    pr_info("set %s as %s\n", pctldev->desc->pins[group].name,
           global_functions_desc[selector].func_name);
    return 0;
}


// virtual_pmx_gpio_request_enable
// 把 某个引脚配置成 GPIO， 实际上是把 某个引脚配置成 输入，要操作寄存器
static int virtual_pmx_gpio_request_enable(struct pinctrl_dev *pctldev, struct pinctrl_gpio_range *range, unsigned offset)
{
    pr_info("set and enable pin %s as GPIO\n", pctldev->desc->pins[offset].name);
    return 0;
}

static const struct pinmux_ops virtual_pmx_ops = {
    .get_functions_count = virtual_pmx_get_funcs_count,
    .get_function_name = virtual_pmx_get_func_name,
    .get_function_groups = virtual_pmx_get_groups,
    .set_mux = virtual_pmx_set,
    .gpio_request_enable = virtual_pmx_gpio_request_enable,// gpio_request_enable  是用来 申请gpio的
};

static int virtual_pinconf_get(struct pinctrl_dev *pctldev, unsigned pin_id,
                              unsigned long *config)
{
    *config = global_configs[pin_id];
    return 0;
}

static int virtual_pinconf_set(struct pinctrl_dev *pctldev, unsigned pin_id,
                              unsigned long *configs, unsigned num_configs)
{
    if (num_configs != 1)
        return -EINVAL;
    global_configs[pin_id] = configs[0];
    pr_info("config %s as 0x%lx\n", pctldev->desc->pins[pin_id].name,
           global_configs[pin_id]);
    return 0;
}

static void virtual_pinconf_dbg_show(struct pinctrl_dev *pctldev, struct seq_file *s,
                                    unsigned pin_id)
{
    seq_printf(s, "0x%lx", global_configs[pin_id]);
}

static void virtual_pinconf_group_dbg_show(struct pinctrl_dev *pctldev,
                                          struct seq_file *s, unsigned pin_id)
{
    seq_printf(s, "0x%lx", global_configs[pin_id]);
}

static const struct pinconf_ops virtual_pinconf_ops = {
    .pin_config_get = virtual_pinconf_get,
    .pin_config_set = virtual_pinconf_set,
    .pin_config_dbg_show = virtual_pinconf_dbg_show,
    .pin_config_group_dbg_show = virtual_pinconf_group_dbg_show,
};

static int virtual_controller_driver_probe(struct platform_device *pdev)
{
    struct pinctrl_desc *pctrl;

    dev_info(&pdev->dev, "virtual_controller_driver_probe\n");

    pctrl = devm_kzalloc(&pdev->dev, sizeof(*pctrl), GFP_KERNEL);
    if (!pctrl) {
        dev_err(&pdev->dev, "Failed to allocate pctrl\n");
        return -ENOMEM;
    }

    pctrl->name = dev_name(&pdev->dev);
    pctrl->owner = THIS_MODULE;
    pctrl->pins = pins;
    pctrl->npins = ARRAY_SIZE(pins);
    pctrl->pctlops = &virtual_pctrl_ops;
    pctrl->pmxops = &virtual_pmx_ops;
    pctrl->confops = &virtual_pinconf_ops;

    global_pinctrl_dev = devm_pinctrl_register(&pdev->dev, pctrl, NULL);
    if (IS_ERR(global_pinctrl_dev)) {
        dev_err(&pdev->dev, "Failed to register pinctrl: %ld\n",
                PTR_ERR(global_pinctrl_dev));
        return PTR_ERR(global_pinctrl_dev);
    }
    dev_info(&pdev->dev, "Registered pinctrl device\n");

    // 创建 debugfs 目录（可选）
    debugfs_root = debugfs_create_dir("pinctrl/virtual_pincontroller", NULL);
    if (IS_ERR(debugfs_root)) {
        dev_warn(&pdev->dev, "Failed to create debugfs dir: %ld, continuing without debugfs\n",
                 PTR_ERR(debugfs_root));
        debugfs_root = NULL;
    } else {
        dev_info(&pdev->dev, "Created debugfs directory: pinctrl/virtual_pincontroller\n");

        // 创建 pinctrl-configs 文件
        if (!debugfs_create_file("pinctrl-configs", 0664, debugfs_root, NULL,
                                 &pinctrl_configs_fops)) {
            dev_warn(&pdev->dev, "Failed to create pinctrl-configs file, continuing without it\n");
            debugfs_remove_recursive(debugfs_root);
            debugfs_root = NULL;
        } else {
            dev_info(&pdev->dev, "Created pinctrl-configs debugfs file\n");
        }
    }

    return 0;
}

static int virtual_controller_driver_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "virtual_controller_driver_remove\n");
    if (debugfs_root)
        debugfs_remove_recursive(debugfs_root);
    return 0;
}

static const struct of_device_id virtual_controller_driver_of_match[] = {
    { .compatible = "kingnan,virtual_controller_driver", },
    { },
};

static struct platform_driver virtual_controller_driver_device_driver = {
    .probe      = virtual_controller_driver_probe,
    .remove     = virtual_controller_driver_remove,
    .driver     = {
        .name   = "virtual_controller_driver",
        .of_match_table = virtual_controller_driver_of_match,
    }
};

static int __init virtual_controller_driver_init(void)
{
    return platform_driver_register(&virtual_controller_driver_device_driver);
}

static void __exit virtual_controller_driver_exit(void)
{
    platform_driver_unregister(&virtual_controller_driver_device_driver);
}

module_init(virtual_controller_driver_init);
module_exit(virtual_controller_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Virtual pinctrl driver for testing");