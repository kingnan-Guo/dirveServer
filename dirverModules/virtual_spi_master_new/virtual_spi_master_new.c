#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>

static struct spi_master *gloabl_virtual_master;// 全局虚拟 SPI 主设备
static struct spi_bitbang* gloabl_virtual_bitbang;// 全局虚拟 SPI bitbang
static struct completion gloabl_virtual_xfer_done;// 全局传输完成信号量




/*
    虚拟的。spi_virtual_transfer 模拟 硬件操作 成功
    会有 唤醒操作
    1. 重新 初始化完成信号量
    2. 唤醒等待的 线程
*/
static int spi_virtual_transfer(struct spi_device *spi, struct spi_transfer *transfer)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    int timeout;

    //  重新初始化 完成信号量
    reinit_completion(&gloabl_virtual_xfer_done);



    // 模拟 硬件传输 直接完成
    complete(&gloabl_virtual_xfer_done);

    // 由于 直接模拟传输成功 所以直接返回传输长度
    //  唤醒等待的 线程
    timeout = wait_for_completion_timeout(&gloabl_virtual_xfer_done, 100);// 等待完成信号量，超时时间为 100 毫秒
    if(!timeout){
        dev_err(&spi->dev, "spi_virtual_transfer timeout.\n");
        return -ETIMEDOUT; // 超时
    }
    return transfer->len; // 返回传输长度
    return 0;
}

static void	spi_virtual_chipselect(struct spi_device *spi, int is_on)
{
}

/*-------------------------------------------------------------------------*/
static int virtual_spi_master_probe(struct platform_device *pdev)

{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    struct spi_master *master; // 获取 SPI 主设备
    int ret;
    /* 1 分配 / 设置 / 注册 spi_master */
    // ，sizeof(struct spi_bitbang)是额外分配的内存大小。
    master = spi_alloc_master(&pdev->dev, sizeof(struct spi_bitbang));// 分配 SPI 主设备
    gloabl_virtual_master = master; // 全局变量赋值
    if(master == NULL){
        dev_err(&pdev->dev, "spi_alloc_master error.\n");
        return -ENOMEM; // 内存分配失败
    }
    gloabl_virtual_master->dev.of_node = pdev->dev.of_node; // 设置设备树节点

    // gloabl_virtual_bitbang = gloabl_virtual_master + 1;// 由于master 分配了 sizeof(struct spi_bitbang)大小的内存，所以bitbang在master后面 
    gloabl_virtual_bitbang = spi_master_get_devdata(master);// spi_master_get_devdata 是一个 Linux 内核提供的宏或函数，用于获取 spi_master 结构体中存储的额外数据的指针。spi_alloc_master 在分配 spi_master 时，会将额外分配的内存（sizeof(struct spi_bitbang)）的地址存储在 spi_master 结构体的某个字段中（通常是 dev.driver_data）

    //  先初始化一下 
    init_completion(&gloabl_virtual_xfer_done);// 初始化完成信号量


    /*
    设置 spi_masker
    1 spi master 使用默认的函数
    2 分配 设置  spi_bitbang 结构体， 主要目的是 实现 内部的 txrx_bufs 函数
    3 spi_master 要能找到 spi_bitbang
    
    */

    gloabl_virtual_bitbang->master = master; // 设置 bitbang 的 master
    gloabl_virtual_bitbang->txrx_bufs = spi_virtual_transfer; // 设置 bitbang 的 txrx_bufs 函数;发送 接收 数据的函数
    gloabl_virtual_bitbang->chipselect = spi_virtual_chipselect; // 设置 bitbang 的 chipselect 函数

    gloabl_virtual_master->dev.of_node = pdev->dev.of_node; // 设置设备树节点

    // 注册 bitbang 启动
    ret = spi_bitbang_start(gloabl_virtual_bitbang);// 启动 bitbang
    /**
     spi_bitbang_start 中 包含 注册 spi_master 的函数
     spi_master->transfer_one = spi_bitbang_transfer_one; // 设置传输函数
        spi_bitbang_transfer_one 只 是调用 spi_bitbang->txrx_bufs 函数 , 就是上面提供的 spi_virtual_transfer 函数
     
     spi_master->setup = spi_bitbang_setup; // 设置初始化函数


     */
	if (ret) {
		printk("bitbang start failed with %d\n", ret);
		return ret;
	}
	return 0;
}

static int virtual_spi_master_remove(struct platform_device *pdev)
{

    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    spi_bitbang_stop(gloabl_virtual_bitbang); // 停止 bitbang
    spi_master_put(gloabl_virtual_master); // 释放 spi_master
    return 0;
}


static const struct of_device_id virtual_spi_master_dt_ids[] = {
	{ .compatible = "virtual_spi_master", },
	{ /* sentinel */ }
};



static struct platform_driver virtual_spi_master_spi_driver = {
	.driver = {
		.name =		"virtual_spi_master",
		.of_match_table = virtual_spi_master_dt_ids,
	},
	.probe =	virtual_spi_master_probe,
	.remove =	virtual_spi_master_remove,

};

/*-------------------------------------------------------------------------*/

static int  virtual_spi_master_init(void)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return platform_driver_register(&virtual_spi_master_spi_driver);
}


static void  virtual_spi_master_exit(void)
{

    platform_driver_unregister(&virtual_spi_master_spi_driver);
}
module_init(virtual_spi_master_init);
module_exit(virtual_spi_master_exit);


MODULE_LICENSE("GPL");
MODULE_ALIAS("virtual_spi_master");
