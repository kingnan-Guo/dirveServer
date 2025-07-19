#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/acpi.h>

#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include <linux/uaccess.h>
#include <linux/gpio/consumer.h>


//为0 表示命令，为1表示数据
#define OLED_CMD 	0
#define OLED_DATA 	1

#define OLED_IOC_INIT 			123
#define OLED_IOC_SET_POS 		124


static struct  spi_device *oled_spi;
static int major;
static struct gpio_desc *dc_gpio;

static ssize_t
spidev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{

	return count;
}


static long
spidev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{

	return 0;
}





static int spidev_release(struct inode *inode, struct file *filp)
{
	// struct spidev_data	*spidev;
	return 0;
}

static const struct file_operations spidev_fops = {
	.owner =	THIS_MODULE,
	/* REVISIT switch to aio primitives, so that userspace
	 * gets more complete API coverage.  It'll simplify things
	 * too, except for the locking.
	 */
	.write =	spidev_write,
	// .read =		spidev_read,
	.unlocked_ioctl = spidev_ioctl,
	// .compat_ioctl = spidev_compat_ioctl, // 这个 是 32 位程序调用 64 位的 ioctl
	.release =	spidev_release,
};

/*-------------------------------------------------------------------------*/

/* The main reason to have this class is to make mdev/udev create the
 * /dev/spidevB.C character device nodes exposing our userspace API.
 * It also simplifies memory management.
 */




static struct class *spidev_class;



static const struct of_device_id spidev_dt_ids[] = {
	{ .compatible = "spi_dev_oled" },
	{},
};
MODULE_DEVICE_TABLE(of, spidev_dt_ids);




/*-------------------------------------------------------------------------*/

static int spidev_probe(struct spi_device *spi)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    /* 1 记录 spi dev */
    oled_spi = spi;

    /* 2 注册字符设备 */
    major  = register_chrdev(0, "spi_dev_oled", &spidev_fops);
    spidev_class = class_create( "spi_dev_oled");
    device_create(spidev_class, NULL, MKDEV(major, 0), NULL, "spi_dev_oled");
    
    /* 3 */
	return 0;
}

static void spidev_remove(struct spi_device *spi)
{
	// struct spidev_data	*spidev = spi_get_drvdata(spi);

	/* 1 注销字符设备 */
    device_destroy(spidev_class, MKDEV(major, 0));
    class_destroy(spidev_class);
	unregister_chrdev(major, "spi_dev_oled");
    /* 2 释放 spi dev */


}

static struct spi_driver spidev_spi_driver = {
	.driver = {
		.name =		"spidev,spi_dev_oled",
		.of_match_table = spidev_dt_ids,
	},
	.probe =	spidev_probe,
	.remove =	spidev_remove,
	// .id_table =	spidev_spi_ids,

	/* NOTE:  suspend/resume methods are not necessary here.
	 * We don't do anything except pass the requests to/from
	 * the underlying controller.  The refrigerator handles
	 * most issues; the controller driver handles the rest.
	 */
};

/*-------------------------------------------------------------------------*/

static int __init spidev_init(void)
{
	int status;

	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	status = spi_register_driver(&spidev_spi_driver);
	if (status < 0) {

	}
	return status;
}
module_init(spidev_init);

static void __exit spidev_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	spi_unregister_driver(&spidev_spi_driver);

}
module_exit(spidev_exit);


MODULE_LICENSE("GPL");
MODULE_ALIAS("spi:spidev");


