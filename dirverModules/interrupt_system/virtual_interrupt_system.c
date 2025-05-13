#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/random.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/gpio/driver.h>
/* FIXME: for gpio_get_value() replace this with direct register read */
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/bug.h>
#include <linux/random.h>




static int virtual_intc_probe(struct platform_device *pdev)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);


    /** 
     * 核心： 分配 / 设置 / 注册  irq_domain
     *   构造 irq_domain 有三种方法
     *      1 . irq_domain_add_legacy()     传统方式 已过时 【这次使用 传统的方式】
     *      2. irq_domain_add_linear()      线性方式
     *      3. irq_domain_add_hierarchy()   层级方式
     * */



    /* 从平台设备里面 获得 中断号: 从设备树里获得 n */

    /* 去设置 链式的 handler: 设置 它的 irq_desc[].handle_irq */


	return 0;
}


static int virtual_interrupt_remove(struct platform_device *pdev)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}



static const struct of_device_id virtual_interrupt_of_match[] = {
    { .compatible = "kingnan,virtual_irq_controller", },
    { },
};

static struct platform_driver virtual_interrupt_driver = {
	.probe		= virtual_interrupt_probe,
	.remove		= virtual_interrupt_remove,
	.driver		= {
		.name	= "virtual_irq_controller",
		.of_match_table = of_match_ptr(virtual_interrupt_of_match),
	}
};

/* 1. 入口函数 */
static int __init virtual_interrupt_init(void)
{	
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	/* 1.1 注册一个platform_driver */
	return platform_driver_register(&virtual_interrupt_driver);
}

/* 2. 出口函数 */
static void __exit virtual_interrupt_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	/* 2.1 反注册platform_driver */
	platform_driver_unregister(&virtual_interrupt_driver);
}

module_init(virtual_interrupt_init);
module_exit(virtual_interrupt_exit);

MODULE_LICENSE("GPL");

