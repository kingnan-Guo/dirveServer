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
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    struct spi_message *mesg;
    // 循环处理 SPI 消息队列
    while (!list_empty(&gloabl_virtual_master->queue)) // 判断队列是否为空
    {
        mesg = list_entry(gloabl_virtual_master->queue.next, struct spi_message, queue); // 获取队列中的第一个消息

        list_del_init(&mesg->queue);// 从队列中删除该消息
		/* 假装硬件传输已经完成 */
        //  这里 会触发硬件传输

        mesg->status = 0;
        if(mesg->complete) {
            // 如果有完成回调函数，则调用它
            // 传输完成后会把 status 设置为 0
            mesg->complete(mesg->context);// 这个是 
        }

    }
    
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

    mesg->actual_length = 0; // 实际长度为 0
    mesg->status = -EINPROGRESS; // 状态为进行中
    //  把消息放入对列
    list_add_tail(&mesg->queue, &spi->master->queue); // 把消息放入对列 尾部


    // 启动对列

    schedule_work(&gloabl_virtual_ws); // 启动工作队列



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

    gloabl_virtual_master->transfer = spi_virtual_transfer; // 设置传输函数
    //  初始化 工作队列
    INIT_WORK(&gloabl_virtual_ws, spi_virtual_work);
    // master->dev.of_node = pdev->dev.of_node; // 设置设备树节点
    gloabl_virtual_master->dev.of_node = pdev->dev.of_node; // 设置设备树节点



    // 注册 spi master
    ret = spi_register_master(gloabl_virtual_master);
    if(ret < 0){
		printk(KERN_ERR "spi_register_master error.\n");
		spi_master_put(gloabl_virtual_master);
		return ret;
    }




	return 0;
}

static int virtual_spi_master_remove(struct platform_device *pdev)
{
    spi_unregister_master(gloabl_virtual_master); // 注销 SPI 主设备
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
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
    return platform_driver_register(&virtual_spi_master_spi_driver);
}


static void  virtual_spi_master_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    platform_driver_unregister(&virtual_spi_master_spi_driver);
}
module_init(virtual_spi_master_init);
module_exit(virtual_spi_master_exit);


MODULE_LICENSE("GPL");
MODULE_ALIAS("virtual_spi_master");
