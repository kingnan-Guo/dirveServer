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


// virtual_interrupt_irq_map
static int virtual_interrupt_irq_map(struct irq_domain *d, unsigned int virq, irq_hw_number_t hw){
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    /**
     * 1 给 virtual_irq 提供 处理函数 ： 也就是 irq_desc[].handle_irq ： handle_c ；用来 处理 中断
     * 2 提供 irq_chip 函数， 也就是 irq_desc[].irq_chip ： chip ：用来 mask/unmask中断
     */

    irq_set_chip_data(virq, d->host_data);// 设置 处理函数
    // 内核提供的 handle_edge_irq 是 用来 处理 中断的, ，他可以 mask /unmask中断，也就是 让 handle_edge_irq 作为 handle_c
    irq_set_chip_and_handler(virq, &virtual_interrupt_irq_chip, handle_edge_irq); // 设置 处理函数
    
    
    return 0;
}


static const struct irq_domain_ops virtual_interrupt_domain_ops = {
	.xlate = irq_domain_xlate_onetwocell,// xlate 用来解析设备树 的属性 ： irq_domain_xlate_onetwocell 函数是 用来解析 1个 或者 2 个 cell， 
	.map   = virtual_interrupt_irq_map,// 用来分配 virq
};








static int virtual_interrupt_get_hwirq(void){
    return 0;
}

/**
 * 读取寄存器，确定发生的是哪一个中断
 * 分辨是哪一个 hardware_irq  调用 对应的 irq_desc[].handle_irq 
 * 
 *  1 先屏蔽中断 chained_irq_enter
 * 
 *  2 读取寄存器，确定发生的是哪一个中断
 * 
 * 
 *  6 恢复中断  chained_irq_exit 
 */
static void virtual_interrupt_irq_handler(struct irq_desc *desc)
{
    // u32 irq_stat;
    int hardware_irq;// 硬件 中断号

    
    struct irq_chip *chip = irq_desc_get_chip(desc);// 获得中断控制器

    chained_irq_enter(chip, desc);// 先屏蔽中断


    /* a. 分辨中断 */

    hardware_irq = virtual_interrupt_get_hwirq();// 读取 硬件寄存器，分辨 确定发生的是哪一个中断

    /* b. 调用 irq_desc[].handle_irq 也就是 handle_c */
    /// 根据 domain 找到虚拟中断号， 调用处理函数
    int virtual_irq = irq_find_mapping(virtual_interrupt_domain, hardware_irq);// 通过 hardware_irq 在 virtual_interrupt_domain 找到虚拟中断号
    generic_handle_irq(virtual_irq);//通用的 处理 中断； 就是去调用 handle_c 函数


    chained_irq_exit(chip, desc);//  恢复中断



}

static int virtual_interrupt_probe(struct platform_device *pdev)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    struct device_node *np = pdev->dev.of_node;// 获得 设备树的节点
    int irq_to_parent;//中断号
    int irq_base; //虚拟中断号
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



    /* 1 从平台设备里面 获得 中断号: 从设备树里获得 n */
    irq_to_parent = platform_get_irq(pdev, 0);//  从 设备树里  获得 第 0 个中断号
    printk("%s %s %d 从平台设备里面 获得 中断号: %d\n", __FILE__, __FUNCTION__, __LINE__, irq_to_parent);
    if (irq_to_parent < 0) {
        printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        return irq_to_parent;
    }
    /* 
    2
    
    去设置 链式的 handler: 设置 它的 irq_desc[].handle_irq ； 
        也就是 handleX 要由中断控制器程序提供，handleX 是读取 寄存器，确定发生的是哪一个中断， 然后调用对应的  handleC   ； 
    分辨是哪一个 hardware_irq  调用 对应的 irq_desc[].handle_irq 
     
    */
    //  为 irq_to_parent 设置一个函数
    irq_set_chained_handler_and_data(irq_to_parent, virtual_interrupt_irq_handler, NULL);





    /* 3 分配 / 设置 / 注册  irq_domain */
    // // 对于每一个 硬件中断号， 都要在  内核 有  里注册一个 irq_desc ，在 irq_domain 里
    // // 分配 irq_desc
    // irq_base = irq_alloc_descs(-1, 0, 4, numa_node_id());// 分配 4 个中断号; numa_node_id() 是一个 NUMA 相关的函数，返回当前 CPU 所在的 NUMA 节点 ID
    // // 使用 传统方式 
    // // 为 np  这个平台创建 4  个中断号的 irq_domain
    // virtual_interrupt_domain = irq_domain_add_legacy(np, 4, irq_base, 0, &virtual_interrupt_domain_ops, NULL);

    //irq_domain_add_linear 在使用的 时候 才去分配 
    /**
     * 在 内核解析设备树的 时候 才会去 分配 irq_desc 得到 virq
     * (hardware_irq, virq) = irq_domain_alloc_irqs(virq_domain, 1, NULL); 存入 domain_irqs
     * 
     */
    irq_domain_add_linear(np, 4, &virtual_interrupt_domain_ops, NULL);

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

