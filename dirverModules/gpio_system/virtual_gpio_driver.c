
#include <linux/module.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>
#include <linux/slab.h>
#include <linux/regmap.h>


static struct gpio_chip * global_virtual_gpio;// 
static int global_gpio_val = 0; // 用来保存 gpio 的值


// virtual_gpio_direction_output 给 gpio_chip 结构体 设置输出 高低电平， offset 是 gpio_chip 结构体的偏移量
// 把 某个引脚配置成 输入 ，还去甚至 输出的 value
static int virtual_gpio_direction_output(struct gpio_chip *chip, unsigned offset, int value){
    printk("set pin %d as output %s\n", offset, value ? "high" : "low");
    return 0;
}


// virtual_gpio_direction_input 给 gpio_chip 结构体 设置输入
static int virtual_gpio_direction_input(struct gpio_chip *chip, unsigned offset){
    printk("set pin %d as input\n", offset);
    return 0;
}

// virtual_gpio_get_value 获得 gpio 的值
static int virtual_gpio_get_value(struct gpio_chip *chip, unsigned offset){
    int val;// 读取 gpio 的值
    val = (global_gpio_val & (1 << offset)) ? 1 : 0; // 读取 gpio 的值
    printk("get pin %d, it's val = %d\n", offset, val); // 打印 gpio 的值
    return val;
}


// virtual_gpio_set_value 设置 gpio 的值
static int virtual_gpio_set_value(struct gpio_chip *chip, unsigned offset, int value){

    printk("set pin %d as %s\n", offset, value ? "high" : "low");
    if(value){
        global_gpio_val |= (1 << offset); // 设置 gpio 的值
    } else{
        global_gpio_val &= ~(1 << offset); // 清除 gpio 的值
    }

    return 0;
}

static int virtual_gpio_probe(struct platform_device *pdev)
{
    int ret;
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);


    /** 分配 设置 注册 一个 gpio_chip */
    /** 1 分配  gpio_chip */
    global_virtual_gpio = devm_kzalloc(&pdev->dev, sizeof(*global_virtual_gpio), GFP_KERNEL);


    /** 2 设置 gpio_chip 结构体 只设置一些必须的 */
    /** 2.1 设置函数 */
    global_virtual_gpio->label = pdev->name;
    global_virtual_gpio->owner = THIS_MODULE;
    global_virtual_gpio->direction_output = virtual_gpio_direction_output; // 把某一个引脚配置为 输出，就是把 某一个 decs 的偏移量 配置为 1
    global_virtual_gpio->direction_input = virtual_gpio_direction_input; // 把某一个引脚配置为 输入，就是把 某一个 decs 的偏移量 配置为 0
    global_virtual_gpio->get = virtual_gpio_get_value; // 读取 gpio 的值
    global_virtual_gpio->set = virtual_gpio_set_value; // 设置 gpio 的值

    global_virtual_gpio->parent = &pdev->dev; // 设备树的父节点


    /** 2.2 设置base、ngpio值 */

    global_virtual_gpio->base = -1; // gpio 的起始地址,让 系统分配

    unsigned int val;
    ret = of_property_read_u32(pdev->dev.of_node, "ngpios", val); // 从设备树里 获得 数据
    global_virtual_gpio->ngpio = val; // gpio 的数量，从设备树里获得


    /** 3 注册  gpio_chip */
    ret = devm_gpiochip_add_data(global_virtual_gpio, NULL);

	return 0;
}


static int virtual_gpio_remove(struct platform_device *pdev)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static const struct of_device_id virtual_gpio_of_match[] = {
	{ .compatible = "kingnan,virtual_gpio", },
	{ },
};

static struct platform_driver virtual_gpio_driver = {
	.probe		= virtual_gpio_probe,
	.remove		= virtual_gpio_remove,
	.driver		= {
		.name	= "_virtual_gpio",
		.of_match_table = of_match_ptr(virtual_gpio_of_match),
	}
};

/* 1. 入口函数 */
static int __init virtual_gpio_init(void)
{	
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	/* 1.1 注册一个platform_driver */
	return platform_driver_register(&virtual_gpio_driver);
}

/* 2. 出口函数 */
static void __exit virtual_gpio_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	/* 2.1 反注册platform_driver */
	platform_driver_unregister(&virtual_gpio_driver);
}

module_init(virtual_gpio_init);
module_exit(virtual_gpio_exit);

MODULE_LICENSE("GPL");