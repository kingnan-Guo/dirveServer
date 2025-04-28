#include <linux/module.h>

#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/virtual_controller_driver.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>
#include <dt-bindings/input/gpio-keys.h>

static struct pinctrl_dev *global_pinctrl_dev;

static const struct pinctrl_pin_desc pins[] = {
    PINCTRL_PIN(0, "virtual_controller_driver_pin0"),
    PINCTRL_PIN(1, "virtual_controller_driver_pin1"),
    PINCTRL_PIN(2, "virtual_controller_driver_pin2")
}


// 用来配置复用有哪些功能
static const char * functions[] = {
    "gpio",
    "i2c",
    "uart",
    "spi",
}




// virtual_get_groups_count
static int virtual_get_groups_count(struct pinctrl_dev *pctldev)
{
    return pctldev->desc->npins;// 获取引脚组数量, 就是 下面的 设置的 npins
}


static const char *virtual_get_group_name(struct pinctrl_dev *pctldev, unsigned selector){
    // selector 是引脚组的索引
    return pctldev->desc->pins[selector].name;// 获取引脚组名称
}

// virtual_get_group_pins 取出 引脚组 的引脚 序号 和 数量，这里每一组 就是一个引脚
static int virtual_get_group_pins(struct pinctrl_dev *pctldev, unsigned selector, const char **pins, unsigned *num_pins){

    if(select >= pctldev->desc->npins){// 引脚组索引 不能超出 个数
        return -EINVAL;

    }
    *pins = &pctldev->desc->pins[selector].number;// 获取引脚组的 引脚 序号
    *num_pins = 1;// 引脚数量

    return -EINVAL;
}

// virtual_pin_dbg_show 打印 信息
static void virtual_pin_dbg_show(struct pinctrl_dev *pctldev, struct seq_file *s, unsigned offset){
    // seq_printf(s, "pin %s\n", pctldev->desc->pins[offset].name);
    seq_printf(s, "pin %s\n", dev_name(pctldev->dev));// 打印 设备名称
}

// virtual_dt_node_to_map
static int virtual_dt_node_to_map(struct pinctrl_dev *pctldev, struct device_node *np, struct pinctrl_map **map, unsigned long *nmaps){
    return -EINVAL;
}


// virtual_dt_free_map
static void virtual_dt_free_map(struct pinctrl_dev *pctldev, struct pinctrl_map *map, unsigned long nmaps){
    kfree(map);// 释放 map
}

static const struct pinctrl_ops virtual_pctrl_ops = {
	.get_groups_count = virtual_get_groups_count,// 获取引脚组数量
	.get_group_name = virtual_get_group_name,// 获取引脚组名称
	.get_group_pins = virtual_get_group_pins,// 获取引脚组引脚
	.pin_dbg_show = virtual_pin_dbg_show,// 调试信息
	.dt_node_to_map = virtual_dt_node_to_map,// 节点信息, 把设备树中 的 节点 转换为 一系列的 pinctl_map 结构体
	.dt_free_map = virtual_dt_free_map,// 释放节点信息

};




/** 复用函数  */

// virtual_pmx_get_funcs_count
static int virtual_pmx_get_funcs_count(struct pinctrl_dev *pctldev){
    return ARRAY_SIZE(functions);// 获取复用函数数量
}

// virtual_pmx_get_func_name
static const char *virtual_pmx_get_func_name(struct pinctrl_dev *pctldev, unsigned selector){
    return functions[selector];// 获取复用函数名称
}


// virtual_pmx_get_groups
static int virtual_pmx_get_groups(struct pinctrl_dev *pctldev, unsigned selector, const char **groups, unsigned *num_groups){
    

    // *groups = 
    // num_groups = 1;
}



// virtual_pmx_get_funcs_count 复用 引脚 配置
static const struct pinmux_ops virtual_pmx_ops = {
	.get_functions_count = virtual_pmx_get_funcs_count,// 获取复用函数数量, 知道 支持 多少个 功能
	.get_function_name = virtual_pmx_get_func_name,// 获取复用函数名称， 取出 每一个 功能的 名字
	.get_function_groups = virtual_pmx_get_groups,// 获取复用函数组， 每个都是独立一组
	.set_mux = virtual_pmx_set,// 设置复用函数
};




// virtual_controller_driver_probe
static int virtual_controller_driver_probe(struct platform_device *pdev){
    dev_info(&pdev->dev, "virtual_controller_driver_probe\n");

    struct pinctrl_desc *pictrl;

    /** 1 分配  pinctrl_desc */
    pictrl = devm_kzalloc(&pdev->dev, sizeof(*pinctrl), GFP_KERNEL);



    /** 2 设置  pinctrl_desc */
    
    // 引脚的一些 主信息
    pictrl->name = dev_name(&pdev->dev);// 设置name
    pictrl->owner = THIS_MODULE;
    pictrl->pins = pins;//支持 哪些 引脚
    pictrl->npins = ARRAY_SIZE(pins);// 引脚数量
    pictrl->pctlops = &virtual_pctrl_ops;// pinctrl_ops 提供一个结构体


    // 设置引脚的 复用函数 pin mux
    pictrl->pmxops = &virtual_pmx_ops;// 复用函数



    /** 3 注册  pinctrl_desc */
    global_pinctrl_dev = devm_pinctrl_register(
        &pdev->dev, pictrl, NULL
    ); 
    

    return 0;
}


// virtual_controller_driver_of_match
static const struct of_device_id virtual_controller_driver_of_match[] = {
    { .compatible = "kingnan,virtual_controller_driver", },
    { },
}

static void virtual_controller_driver_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "virtual_controller_driver_remove\n");
    /* 1.1 反注册platform_driver */
    // platform_driver_unregister(&virtual_controller_driver_device_driver);
}

static struct platform_driver virtual_controller_driver_device_driver = {
	.probe		= virtual_controller_driver_probe,
	.remove	= virtual_controller_driver_remove,
	.driver		= {
		.name	= "virtual_controller_driver",
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