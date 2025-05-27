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

static struct irq_domain *virtual_interrupt_domain;



static void virtual_interrupt_irq_ack(struct irq_data *data)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
}

static void virtual_interrupt_irq_mask(struct irq_data *data)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
}

static void virtual_interrupt_irq_mask_ack(struct irq_data *data)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
}

static void virtual_interrupt_irq_unmask(struct irq_data *data)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
}

static void virtual_interrupt_irq_eoi(struct irq_data *data)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
}


static struct irq_chip virtual_interrupt_irq_chip = {
	.name			= "virtual_irq_controller",
	.irq_ack	   = virtual_interrupt_irq_ack	   ,
	.irq_mask	   = virtual_interrupt_irq_mask	   ,
	.irq_mask_ack  = virtual_interrupt_irq_mask_ack ,
	.irq_unmask    = virtual_interrupt_irq_unmask   ,
	.irq_eoi	   = virtual_interrupt_irq_eoi	   ,
};








/**
 * virtual_interrupt_domain_alloc
 * 要为 irq_desc 提供 handleA irq_data irq_chip
 * 
 */

static int virtual_interrupt_domain_alloc(struct irq_domain *domain, unsigned int irq, unsigned int nr_irqs, void *data)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    struct irq_fwspec *fwspec = data; // 设备树节点 和 中断号
    struct irq_fwspec parent_fwspec; // 父设备树节点 和 中断号
    irq_hw_number_t hwirq; // 硬件中断号

    /** 设置irq_desc[irq] */

    /** 1 设置 irq_desc[irq].irq_data 里面含有 virtual_interrupt 的 irq_chip */
    hwirq = fwspec->param[0]; // 硬件中断号

    // nr_irqs 是 中断号的数量
    for(int i = 0; i < nr_irqs; i++){
        // 设置 硬件中断号 和 chip
        // 这里就是 设置irq_desc[irq] 的 irq_data
        irq_domain_set_hwirq_and_chip(domain, irq + i, hwirq + i, &virtual_interrupt_irq_chip, NULL);
    }



    /** 2 设置 irq_desc[irq].hander_irq , 可能来自 GIC  */
    // 要看 intc 需要几个参数
    parent_fwspec.fwnode = domain->parent->fwnode;  // 父设备树节点
    // parent_fwspec.param_count = 3; // 参数个数
    // parent_fwspec.param[0] = GIC_SPI; // 中断类型， 这里是 0
    // parent_fwspec.param[1] = fwspec->param[0] + upper_hwirq_base;// 中断号
    // parent_fwspec.param[2] = fwspec->param[1];// 0

    // 但是  rbp 的 intc 需要两个参数
    parent_fwspec.param_count = 2; // 参数个数
    parent_fwspec.param[0] = fwspec->param[0] + upper_hwirq_base;// 中断号
    parent_fwspec.param[1] = fwspec->param[1];// 0
    
    return irq_domain_alloc_irqs_parent(domain, irq, nr_irqs, &parent_fwspec); // 分配中断号

}



// virtual_interrupt_domain_translate
/**
 * 解析 gpio 使用 虚拟中断控制器中的 中断号 和 
 * 
 */
static int virtual_interrupt_domain_translate(struct irq_domain *d, struct irq_fwspec *fwspec, unsigned int *hwirq, unsigned int *type){
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    if(is_of_node(fwspec->node)){
        if(fwspec->param_count != 2){
            return -EINVAL;
        }

        *hwirq = fwspec->param[0];// 硬件中断号
        *type = fwspec->param[1];// 中断类型

    }

    return -EINVAL;
}


/**
 * 1 解析 translate
 * 2 分配 irq_desc
 * 3 设置 alloc ： {
 *                  handleA,
 *                  irq_dataB
 *                  action
 *              }
 * 
 */


static const struct irq_domain_ops virtual_interrupt_domain_ops = {
    .translate = virtual_interrupt_domain_translate, // 设备树节点 和 中断号 转换
    .alloc = virtual_interrupt_domain_alloc, // 分配中断号
    // .free = virtual_interrupt_domain_free, // 释放中断号
};



static int virtual_interrupt_probe(struct platform_device *pdev)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    struct device_node *np = pdev->dev.of_node;// 获得 设备树的节点
    int irq_to_parent;//中断号
    int irq_base; //虚拟中断号

    struct irq_domain *parent_domain; // 父域
    struct device_node *parent; // 父设备树节点


    /** 
     * 核心： 分配 / 设置 / 注册  irq_domain
     *   构造 irq_domain 有三种方法
     *      1 . irq_domain_add_legacy()     传统方式 已过时 【这次使用 传统的方式】
     *      2. irq_domain_add_linear()      线性方式
     *      3. irq_domain_add_hierarchy()   层级方式
     * 
     * 
     * 对于每一个 硬件中断号， 都要在  内核 有 irq_domain 里注册一个 irq_desc
     * */

    // 为节点 找到 父亲； 根据 设备树 中的 interrupt-parent 找到父亲节点
    parent = of_irq_find_parent(np);// 获得父中断号


    parent_domain = irq_find_host(parent);// 查找父域， 也就是查找设备树的节点对应的 irq_domain


    //  static inline struct irq_domain *irq_domain_add_hierarchy(struct irq_domain *parent,
    //     unsigned int flags,
    //     unsigned int size,
    //     struct device_node *node,
    //     const struct irq_domain_ops *ops,
    //     void *host_data)     

     virtual_interrupt_domain = irq_domain_add_hierarchy(parent_domain, 0, 4, np, &virtual_interrupt_domain_ops, NULL);// 32 是虚拟中断号的范围



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

