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

static struct spi_master *gloabl_virtual_master;// 全局虚拟 SPI 主设备
static struct work_struct gloabl_virtual_ws;// 全局工作队列


//  工作
static void spi_virtual_work(struct work_struct *work)
{

}


static int spi_virtual_transfer(struct spi_device *spi, struct spi_message *mesg)
{

    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

#if 1
    // 方法  直接 实现 spi 传输

    // 假装传输完成，直接唤醒

    // 传输完成 后 会把 status 设置为 0
    mesg->status = 0;
    mesg->complete(mesg->context);// 这个是 

#else
    // 方法2 
    // 使用工作队列 启动 spi 传输，等待完成

    //  把消息放入对列

    // 启动对列



#endif
    return 0;
}

/*-------------------------------------------------------------------------*/

static int virtual_spi_master_probe(struct platform_device *pdev)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    struct spi_master *master; // 获取 SPI 主设备
    int ret;
    /* 1 分配 / 设置 / 注册 spi_master */
    master = spi_alloc_master(&pdev->dev, 0);// 分配 SPI 主设备
    gloabl_virtual_master = master; // 全局变量赋值
    if(master == NULL){
        dev_err(&pdev->dev, "spi_alloc_master error.\n");
        return -ENOMEM; // 内存分配失败
    }

    master->transfer = spi_virtual_transfer; // 设置传输函数
    // master->dev.of_node = pdev->dev.of_node; // 设置设备树节点
    mster->dev.of_node = pdev->dev.of_node; // 设置设备树节点

    //  初始化 工作队列
    INIT_WORK(&gloabl_virtual_ws, spi_virtual_work);

    // 注册 spi master
    ret = spi_register_master(master);
    if(ret < 0){
		printk(KERN_ERR "spi_register_master error.\n");
		spi_master_put(master);
		return ret;
    }




	return 0;
}

static void virtual_spi_master_remove(struct platform_device *pdev)
{
    spi_unregister_master(gloabl_virtual_master); // 注销 SPI 主设备
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

}

static const struct of_device_id virtual_spi_master_dt_ids[] = {
	{ .compatible = "virtual_spi_master", },
	{ /* sentinel */ }
};



static struct spi_driver virtual_spi_master_spi_driver = {
	.driver = {
		.name =		"virtual_spi_master",
		.of_match_table = virtual_spi_master_dt_ids,
	},
	.probe =	virtual_spi_master_probe,
	.remove =	virtual_spi_master_remove,

};

/*-------------------------------------------------------------------------*/

static int __init virtual_spi_master_init(void)
{
	int status;
    status = platform_driver_register(&virtual_spi_master_spi_driver);
	return status;
}


static void __exit virtual_spi_master_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    platform_driver_unregister(&virtual_spi_master_spi_driver);
}
module_init(virtual_spi_master_init);
module_exit(virtual_spi_master_exit);


MODULE_LICENSE("GPL");
MODULE_ALIAS("virtual_spi_master");
