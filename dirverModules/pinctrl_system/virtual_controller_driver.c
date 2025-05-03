#include <linux/module.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/slab.h>
#include <linux/regmap.h>
#include <linux/seq_file.h> // 添加 seq_file.h 以使用 seq_printf

#include "core.h"
// #include <linux/virtual_controller_driver.h>

static struct pinctrl_dev *global_pinctrl_dev;

static const struct pinctrl_pin_desc pins[] = {
    PINCTRL_PIN(0, "pin0"),
    PINCTRL_PIN(1, "pin1"),
    PINCTRL_PIN(2, "pin2"),
    PINCTRL_PIN(3, "pin3")
};

struct virtual_functions_desc {
    const char *func_name; // 功能名称
    const char **groups;   // 组名称
    int num_groups;        // 组数量
};

// 用来配置复用有哪些功能
// static const char * functions[] = {
//     "gpio",
//     "i2c",
//     "uart",
//     "spi",
// }

static const char *fun0_groups[] = {"pin0", "pin1", "pin2", "pin3"};
static const char *fun1_groups[] = {"pin0", "pin1"};
static const char *fun2_groups[] = {"pin2", "pin3"};

static struct virtual_functions_desc global_functions_desc[] = {
    {"gpio", fun0_groups, 4},
    {"i2c",  fun1_groups, 2},
    {"uart", fun2_groups, 2},
};

static unsigned long global_configs[4]; // 用来配置引脚的配置值

// virtual_get_groups_count
static int virtual_get_groups_count(struct pinctrl_dev *pctldev)
{
    return pctldev->desc->npins; // 获取引脚组数量, 就是 下面的 设置的 npins
}

static const char *virtual_get_group_name(struct pinctrl_dev *pctldev, unsigned selector)
{
    // selector 是引脚组的索引
    return pctldev->desc->pins[selector].name; // 获取引脚组名称
}

// virtual_get_group_pins 取出 引脚组 的引脚 序号 和 数量，这里每一组 就是一个引脚
static int virtual_get_group_pins(struct pinctrl_dev *pctldev, unsigned selector, const unsigned int **pins, unsigned *num_pins)
{
    static unsigned int pin;

    if (selector >= pctldev->desc->npins) { // 引脚组索引 不能超出 个数
        return -EINVAL;
    }
    pin = pctldev->desc->pins[selector].number;
    *pins = &pin; // 获取引脚组的 引脚 序号
    *num_pins = 1; // 引脚数量

    return 0; // 成功返回 0
}

// virtual_pin_dbg_show 打印 信息
static void virtual_pin_dbg_show(struct pinctrl_dev *pctldev, struct seq_file *s, unsigned offset)
{
    seq_printf(s, "pin %s\n", dev_name(pctldev->dev)); // 打印 设备名称
}

// virtual_dt_node_to_map 开始处理 某个设备树的 节点
/** 设备树， 从这设备树里获取信息

virtual_pincontroller{
    compatible = "kingnan,virtual_pincontroller";
    i2cgrp {
        functions = "i2c", "i2c"; // 复用功能
        group = "pin0", "pin1";   // 引脚组
        config = <0x11 0x22>;     // 配置值, 会在 virtual_pinconf_get 中获得到 配置值 
    }
}

在 pinctrl_map  中 
struct pinctrl_map {
	const char *dev_name;
	const char *name;
	enum pinctrl_map_type type;
	const char *ctrl_dev_name;
	union {
		struct pinctrl_map_mux mux; // 引脚复用功能
		struct pinctrl_map_configs configs;
	} data;
};

# 一个 引脚 对应 两个 pinctrl_map
    1、其中 一个 是  引脚 复用 mux ，要把 设备树中 的 pino0 设置为 i2c 功能， 
    2、还需要 另一个 pinctrl_map 来 表示 config 配置值
*/
static int virtual_dt_node_to_map(struct pinctrl_dev *pctldev, struct device_node *np, struct pinctrl_map **map, unsigned *nmaps)
{
    //1、 确定 引脚个数 ， 分配 pinctrl_map 结构体
    int num_pin = 0;
    // char *name; // 未使用，注释掉
    const char *pin; // 储存 
    const char *functions; // 储存 functions
    unsigned int config; // 储存 配置值
    unsigned long *configs; // 指针

    while (1) {
        // 读取 设备树中 group 内的 字符串
        /**
         * of_property_read_string_index
         *  参数
         *      np 设备树节点 ： 读取哪个 device_node 节点 ： np
         *      propname 属性名称：读取哪个 属性 名字 ： group
         *      index 索引： 读取这个 group 属性里的 第几个 值 ： 0
         *      out_value 输出值： 取出来的 值放到 哪里: name
         *   
         *  如果读取到的值 为0 的时候 ，表示读取成功
         */
        if (of_property_read_string_index(np, "group", num_pin, &pin) == 0) {
            // 读取成功
            num_pin++;
        } else {
            break; // 读取失败，跳出循环
        }
    }

    struct pinctrl_map *pctrl_map = NULL;

    // pctrl_map = kmalloc_array(num_pin * 2, sizeof(struct pinctrl_map), GFP_KERNEL); //  num_pin * 2 是因为 要使用 连个 pinctrl_map 来存储信息
    pctrl_map = kmalloc(sizeof(struct pinctrl_map) * num_pin * 2, GFP_KERNEL);//  num_pin * 2 是因为 要使用 连个 pinctrl_map 来存储信息
    
    //2、 逐个 取出 引脚 引脚功能 引脚个数
    for (size_t i = 0; i < num_pin; i++) {
        // get pin function config
        // 使用 of_property_read_string_index 读取 设备树中 group 属性的值
        // 使用 of_property_read_string_index 读取 设备树中 functions 属性的值
        // 使用 of_property_read_u32_index 读取 设备树中 config 属性的值
        of_property_read_string_index(np, "group", i, &pin); // 读取 group 属性的值
        of_property_read_string_index(np, "functions", i, &functions); // 读取 functions 属性的值
        of_property_read_u32_index(np, "config", i, &config); // 读取 config 属性的值

        // 分配 configs 数组指针
        configs = kmalloc(sizeof(*configs), GFP_KERNEL); // 分配 configs 数组指针

        //3、 存入 pinctrl_map 结构体中
        // 每个引脚对应两个 pinctrl_map 结构体
        pctrl_map[i * 2].type = PIN_MAP_TYPE_MUX_GROUP; // 复用功能
        pctrl_map[i * 2].data.mux.function = functions; // 复用功能
        pctrl_map[i * 2].data.mux.group = pin; // 引脚组

        pctrl_map[i * 2 + 1].type = PIN_MAP_TYPE_CONFIGS_PIN; // 要把某一个引脚 配置 成什么值
        pctrl_map[i * 2 + 1].data.configs.group_or_pin = pin; // 引脚组
        pctrl_map[i * 2 + 1].data.configs.configs = configs; // configs 是个数组，当前只有一项

        // 将 读出来的 config  存入 configs
        configs[0] = config; // 读取到的配置值

        pctrl_map[i * 2 + 1].data.configs.num_configs = 1; // 配置数量
    }

    *map = pctrl_map; // 将 pinctrl_map 结构体指针 赋值给 map
    *nmaps = num_pin * 2; // 将 nmaps 赋值为 num_pin * 2
    
    return 0;
}

// virtual_dt_free_map 释放 在 dt_node_to_map 中分配的 pinctrl_map 结构体 内存 等
// 在 注册 pinctrl_map 内存的时候  两个 pinctrl_map 结构体 是一起分配的， 所以放在一起了？？？
static void virtual_dt_free_map(struct pinctrl_dev *pctldev, struct pinctrl_map *map, unsigned num_maps)
{
    for (unsigned i = 0; i < num_maps; i++) {
        if (map[i].type == PIN_MAP_TYPE_CONFIGS_PIN) {
            kfree(map[i].data.configs.configs); // 释放 configs 数组指针
        }
    }
    kfree(map); // 释放整个 map 数组
}

static const struct pinctrl_ops virtual_pctrl_ops = {
    .get_groups_count = virtual_get_groups_count, // 获取引脚组数量
    .get_group_name = virtual_get_group_name,    // 获取引脚组名称
    .get_group_pins = virtual_get_group_pins,    // 获取引脚组引脚
    .pin_dbg_show = virtual_pin_dbg_show,        // 调试信息
    .dt_node_to_map = virtual_dt_node_to_map,    // 节点信息, 把设备树中 的 节点 转换为 一系列的 pinctl_map 结构体
    .dt_free_map = virtual_dt_free_map,          // 释放节点信息
};

// ----------------------

/** 复用函数 */

// virtual_pmx_get_funcs_count
static int virtual_pmx_get_funcs_count(struct pinctrl_dev *pctldev)
{
    return ARRAY_SIZE(global_functions_desc); // 获取复用函数数量
}

// virtual_pmx_get_func_name
static const char *virtual_pmx_get_func_name(struct pinctrl_dev *pctldev, unsigned selector)
{
    return global_functions_desc[selector].func_name; // 获取复用函数名称
}

// virtual_pmx_get_groups
// 获得 某一个 功能里面 组的信息
static int virtual_pmx_get_groups(struct pinctrl_dev *pctldev, unsigned selector, const char * const **groups, unsigned *num_groups)
{
    *groups = global_functions_desc[selector].groups; // 获取 组名称
    *num_groups = global_functions_desc[selector].num_groups; // 获取组数 大小
    return 0;
}

// virtual_pmx_set 设置 复用函数
static int virtual_pmx_set(struct pinctrl_dev *pctldev, unsigned selector, unsigned group)
{
    // 这里设置 复用函数
    // 把 pctldev->desc->pins[group].name 这组 引脚 设置为 global_functions_desc[selector].func_name
    printk("set %s as %s\n", pctldev->desc->pins[group].name, global_functions_desc[selector].func_name);

    // 把 group 某一组 selector 引脚 配置为 某个功能
    return 0;
}

// virtual_pmx_get_funcs_count 复用 引脚 配置
static const struct pinmux_ops virtual_pmx_ops = {
    .get_functions_count = virtual_pmx_get_funcs_count, // 获取复用函数数量, 知道 支持 多少个 功能
    .get_function_name = virtual_pmx_get_func_name,     // 获取复用函数名称， 取出 每一个 功能的 名字
    .get_function_groups = virtual_pmx_get_groups,      // 获取复用函数组， 每个都是独立一组
    .set_mux = virtual_pmx_set,                         // 设置复用函数； 把某一组引脚 配置为 某个功能
};

// ---------------------

// virtual_pinconf_get 取出 某个 引脚的配置值 放入到 
static int virtual_pinconf_get(struct pinctrl_dev *pctldev, unsigned pin_id, unsigned long *config)
{
    *config = global_configs[pin_id]; // 让 *config 配置值 等于 global_configs[pin_id] 里面的值； pin_id 是 引脚的索引
    return 0;
}

// virtual_pinconf_set 设置 引脚的 配置值
static int virtual_pinconf_set(struct pinctrl_dev *pctldev, unsigned pin_id, unsigned long *configs, unsigned num_configs)
{
    if (num_configs != 1) // 如果配置数量不等于 1
        return -EINVAL;   // 返回 错误

    global_configs[pin_id] = *configs; // 根据引脚的 id ： pin_id 设置 引脚的配置值

    // 把 pin_id 这个引脚 配置为 *configs
    printk("config %s as 0x%lx\n", pctldev->desc->pins[pin_id].name, global_configs[pin_id]);

    // 因为没有 实际的 寄存器 配置 
    return 0;
}

// virtual_pinconf_dbg_show 打印 引脚的配置 信息
static void virtual_pinconf_dbg_show(struct pinctrl_dev *pctldev, struct seq_file *s, unsigned pin_id)
{
    seq_printf(s, "0x%lx", global_configs[pin_id]); // 打印 引脚的配置值
}

// virtual_pinconf_group_dbg_show 打印 引脚组 的配置 信息， 现在 一组 引脚 中只有 一个 引脚
static void virtual_pinconf_group_dbg_show(struct pinctrl_dev *pctldev, struct seq_file *s, unsigned pin_id)
{
    seq_printf(s, "0x%lx", global_configs[pin_id]); // 打印 引脚组的配置值
}

// pinconf_ops 引脚配置
static const struct pinconf_ops virtual_pinconf_ops = {
    .pin_config_get = virtual_pinconf_get,             // 获取 引脚的配置值
    .pin_config_set = virtual_pinconf_set,             // 设置 引脚的配置值
    .pin_config_dbg_show = virtual_pinconf_dbg_show,   // 打印 引脚的配置值
    .pin_config_group_dbg_show = virtual_pinconf_group_dbg_show, // 打印 引脚组的配置值
};

// virtual_controller_driver_probe
static int virtual_controller_driver_probe(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "virtual_controller_driver_probe\n");

    struct pinctrl_desc *pictrl;

    /** 1 分配  pinctrl_desc */
    pictrl = devm_kzalloc(&pdev->dev, sizeof(*pictrl), GFP_KERNEL);

    /** 2 设置  pinctrl_desc */
    
    // 引脚的一些 主信息
    pictrl->name = dev_name(&pdev->dev); // 设置name
    pictrl->owner = THIS_MODULE;
    pictrl->pins = pins; // 支持 哪些 引脚
    pictrl->npins = ARRAY_SIZE(pins); // 引脚数量
    pictrl->pctlops = &virtual_pctrl_ops; // pinctrl_ops 提供一个结构体

    // 设置引脚的 复用函数 pin mux
    pictrl->pmxops = &virtual_pmx_ops; // 复用函数

    // 对引脚的配置  pin config
    pictrl->confops = &virtual_pinconf_ops; // 引脚配置操作函数

    /** 3 注册  pinctrl_desc */
    global_pinctrl_dev = devm_pinctrl_register(&pdev->dev, pictrl, NULL); 
    
    return 0;
}

// virtual_controller_driver_of_match
static const struct of_device_id virtual_controller_driver_of_match[] = {
    { .compatible = "kingnan,virtual_controller_driver", },
    { },
};

static int virtual_controller_driver_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "virtual_controller_driver_remove\n");
    /* 1.1 反注册platform_driver */
    // platform_driver_unregister(&virtual_controller_driver_device_driver);
    return 0;
}

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