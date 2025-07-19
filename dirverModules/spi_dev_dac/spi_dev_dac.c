// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Simple synchronous userspace interface to SPI devices
 *
 * Copyright (C) 2006 SWAPP
 *	Andrea Paterniani <a.paterniani@swapp-eng.it>
 * Copyright (C) 2007 David Brownell (simplification, cleanup)
 */

#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/property.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/acpi.h>

#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include <linux/uaccess.h>


static struct spi_device *dac_spi;
static int major;


#define SPI_IOC_WR 123

static long
spidev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int			retval = 0;
	// struct spidev_data	*spidev;
	// struct spi_device	*spi;
    int val;
    int err;
    struct spi_transfer xfer[1];// spi_transfer 结构体数组，表示 SPI 传输的配置和数据。
    struct spi_message msg; // spi_message 结构体，表示 SPI 消息。
    int status; // spi_sync() 函数的返回值，表示 SPI 传输的状态。

    unsigned char tx_buf[2];// 发送缓冲区
    unsigned char rx_buf[2];// 接收缓冲区

    memset(&xfer[0], 0, sizeof(xfer)); // 清零 xfer 数组，确保所有字段都被初始化为零。


    //  copy_from_user
    err = copy_from_user(&val, (const void __user *)arg, sizeof(int)); // 从用户空间读取数据到内核空间

    printk("spidev_ioctl get val from user: %d\n", val);
    // 发起 spi 传输 ; 同时 写 读

    // 1 把 val 修改为正确的格式

    val << 2; // 左移 2 位; bint0, bit1  = 0b00; 让 最底的两位 位 0
    val &=0xFFC; //保留中间的 10 位; 0xFFC = 0b111111111100


    // 2 spi 传输 
    // 2.1 构造 transfer
    tx_buf[1] = val & 0xFF; // 取低 8 位
    tx_buf[0] = (val >> 8) & 0xFF; // 取高 8 位
    xfer[0].tx_buf = tx_buf; // 要发送的数据
    xfer[0].rx_buf = rx_buf; // 接收数据的缓冲区
    xfer[0].len = 2; // 传输的字节


    // 2.2 初始化message  把 xfer 加入 message 尾部
    spi_message_init(&msg);
    spi_messgae_add_tail(&xfer[0], &msg);


    // 2.3 调用 spi_sync 执行传输
    status = spi_sync(dac_spi,&msg);

    // 3 修改读到的数据格式

    // copy_to_user
    err = copy_to_user((const void __user *)arg, &val, sizeof(int));
	return retval;
}




static int spidev_release(struct inode *inode, struct file *filp)
{
	struct spidev_data	*spidev;
	return 0;
}

static const struct file_operations spidev_fops = {
	.owner =	THIS_MODULE,
	/* REVISIT switch to aio primitives, so that userspace
	 * gets more complete API coverage.  It'll simplify things
	 * too, except for the locking.
	 */
	// .write =	spidev_write,
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
	{ .compatible = "spi_dev_dac" },
	{},
};
MODULE_DEVICE_TABLE(of, spidev_dt_ids);




/*-------------------------------------------------------------------------*/

static int spidev_probe(struct spi_device *spi)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	// struct spidev_data	*spidev;
	// int			status;
	// unsigned long		minor;
	/* Allocate driver data */
	// spidev = kzalloc(sizeof(*spidev), GFP_KERNEL);
	/* Initialize the driver data */
	// spidev->spi = spi;


    /* 1 记录 spi dev */
    dac_spi = spi;

    /* 2 注册字符设备 */
    major  = register_chrdev(0, "spi_dev_dac", &spidev_fops);
    spidev_class = class_create(THIS_MODULE, "spi_dev_dac");
    device_create(spidev_class, NULL, MKDEV(major, 0), NULL, "spi_dev_dac");
    
    /* 3 */

	return status;
}

static void spidev_remove(struct spi_device *spi)
{
	struct spidev_data	*spidev = spi_get_drvdata(spi);

	/* 1 注销字符设备 */
    device_destroy(spidev_class, MKDEV(major, 0));
    class_destroy(spidev_class);
	unregister_chrdev(major, "spi_dev_dac");
    /* 2 释放 spi dev */


}

static struct spi_driver spidev_spi_driver = {
	.driver = {
		.name =		"spidev,spi_dev_dac",
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
