#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>


static const struct of_device_id virtual_gpio_clients[] = {
    { .compatible = "kingnan,virtual_gpio_clientdrv" },
    { },
};

/* 1. 定义platform_driver */
static struct platform_driver chip_demo_gpio_driver = {
    .probe      = chip_demo_gpio_probe,
    .remove     = chip_demo_gpio_remove,
    .driver     = {
        .name   = "virtual_gpio_clients",
        .of_match_table = virtual_gpio_clients,
    },
};

/* 2. 在入口函数注册platform_driver */
static int __init virtual_gpio_client_init(void)
{
    int err;
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
    err = platform_driver_register(&chip_demo_gpio_driver); 
	
	return err;
}

/* 3. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 *     卸载platform_driver
 */
static void __exit virtual_gpio_client_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    platform_driver_unregister(&chip_demo_gpio_driver);
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(virtual_gpio_client_init);
module_exit(virtual_gpio_client_exit);

MODULE_LICENSE("GPL");

