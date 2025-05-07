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


static int major = 0; /* 主设备号 */
static char *name = "virtual_gpio_led"; /* 设备名字 */
static struct class *virtual_gpio_client_class = NULL; /* 类 */
// static struct device *virtual_gpio_client_device = NULL; /* 设备 */
static struct gpio_desc *gpiod;


//virtual_gpio_client_open
static int virtual_gpio_client_open(struct inode *inode, struct file *file){
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    // 根据次设备号 初始化
    gpiod_direction_output(gpiod, 0); // 设置 GPIO 为输出模式，初始值为 0

    return 0;
}


//virtual_gpio_client_read
static ssize_t virtual_gpio_client_read(struct file *file, char __user *buf, size_t count, loff_t *offset){
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}



// virtual_gpio_client_write
static ssize_t virtual_gpio_client_write(struct file *file, const char __user *buf, size_t count, loff_t *offset){
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    // 读取用户空间数据
    
    char status;
    if (copy_from_user(&status, buf, sizeof(status))) {
        return -EFAULT;
    }

    // 设置 GPIO 的值
    gpiod_set_value(gpiod, status);


    return 0;
}

// virtual_gpio_client_close
static int virtual_gpio_client_close(struct inode *inode, struct file *file){
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}



/* 定义自己的file_operations结构体                                              */
static struct file_operations virtual_gpio_client_fops = {
	.owner	 = THIS_MODULE,
	.open    = virtual_gpio_client_open,
	.read    = virtual_gpio_client_read,
	.write   = virtual_gpio_client_write,
	.release = virtual_gpio_client_close,
};


static int virtual_gpio_client_probe(struct platform_device *pdev){
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    gpiod = devm_gpiod_get(&pdev->dev, "led", 0);// 获取 设备树中 的  led-gpios = <&gpio_virtual 0 0>;
    if (IS_ERR(gpiod)) {
        dev_err(&pdev->dev, "Failed to get GPIO for led\n");
        return PTR_ERR(gpiod);
    }

    major = register_chrdev(0, name, &virtual_gpio_client_fops);
    virtual_gpio_client_class = class_create("virtual_gpio_led_class");


    char device_name_buf[30];
    int minor = 0;
    snprintf(device_name_buf, sizeof(device_name_buf), "%s_%d", name, minor);
    device_create(virtual_gpio_client_class, NULL, MKDEV(major, minor), NULL, device_name_buf);

    return 0;
}


static int virtual_gpio_client_remove(struct platform_device *pdev){
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    device_destroy(virtual_gpio_client_class, MKDEV(major, 0));
    class_destroy(virtual_gpio_client_class);
    unregister_chrdev(major, name);

    gpiod_put(gpiod);// 释放GPIO

    return 0;
}

static const struct of_device_id virtual_gpio_client[] = {
    { .compatible = "kingnan,gpio_virtual_led" },
    { },
};

/* 1. 定义platform_driver */
static struct platform_driver virtual_gpio_client_driver = {
    .probe      = virtual_gpio_client_probe,
    .remove     = virtual_gpio_client_remove,
    .driver     = {
        .name   = "virtual_gpio_client",
        .of_match_table = virtual_gpio_client,
    },
};

/* 2. 在入口函数注册platform_driver */
static int __init virtual_gpio_client_init(void)
{
    int err;
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
    err = platform_driver_register(&virtual_gpio_client_driver); 
	
	return err;
}

/* 3. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 *     卸载platform_driver
 */
static void __exit virtual_gpio_client_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    platform_driver_unregister(&virtual_gpio_client_driver);
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(virtual_gpio_client_init);
module_exit(virtual_gpio_client_exit);

MODULE_LICENSE("GPL");

