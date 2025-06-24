#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/platform_device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/rational.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>

#include <asm/irq.h>



static struct uart_driver virt_uart_drv = {
    .owner          = THIS_MODULE,
    .driver_name    = "VIRT_UART",
    .dev_name       = "ttyVIRT",
    .major          = 0,
    .minor          = 0,
    .nr             = 1,
}


static int virtual_uart_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    //
    // uart_add_one_port(struct uart_driver *drv, struct uart_port *uport); // 添加一个串口端口
    return 0;
}

static int virtual_client_driver_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}


static const struct of_device_id virtual_uart_of_match[] = {
	{ .compatible = "kingnan,virtual_uart", },
	{ },
};


static struct platform_driver virtual_uart_driver = {
    .probe      = virtual_uart_probe, // virtual_uart_probe,
    .remove     = virtual_uart_remove, // virtual_uart_remove,
    .driver     = {
        .name   = "virtual_uart",
        // .owner  = THIS_MODULE,
        .of_match_table = of_match_ptr(virtual_uart_of_match), // virtual_uart_of_match
    }
}


// 模块加载时执行的函数
static int __init virtual_uart_init(void) {
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    int ret = 0;
    ret = uart_register_driver(&virt_uart_drv);// 注册串口驱动
    if (ret) {
        printk(KERN_ERR "Failed to register UART driver\n");
        return ret;
    }
    ret = platform_driver_register(&virtual_uart_driver);// 注册平台驱动 platform_driver
    return ret;
}

// 模块卸载时执行的函数
static void __exit virtual_uart_exit(void) {
    // printk(KERN_INFO "virtual_uart unregistered.\n");
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    platform_driver_unregister(&virtual_uart_driver);// 卸载平台驱动
    // uart_unregister_driver(&virt_uart_drv);// 卸载串口驱动  
}

// 定义模块的加载和卸载函数
module_init(virtual_uart_init);
module_exit(virtual_uart_exit);

MODULE_LICENSE("GPL");
