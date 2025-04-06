/**
 *  / alloc / set / register        platform_driver
 *  / 分配 / 设置 / 注册              平台驱动 
 * 
 * 
 * 
 */
#include <linux/module.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>

static struct input_dev *global_input_device;
static int input_device_irq;



/* 中断处理函数 */
static irqreturn_t input_device_irq_handler(int irq, void *dev_id){
    /** 读取硬件 得到数据 */

    /** 上报数据  input_event */
    // input_event(global_input_device, EV_KEY, XX, 0);// 上报 按键事件
    // input_sync(global_input_device);// 同步事件

    printk(KERN_INFO "input_device_irq_handler irq %d\n", irq);
    return IRQ_HANDLED;
}


static int input_device_probe(struct platform_device *pdev)
{
    int err;
    struct resource *irq;
    struct device *dev = &pdev->dev;
    /** 从设备树里面 获得 设备信息 */

    /** / alloc / set / register       struct input_device */
    global_input_device = devm_input_allocate_device(dev);// 分配输入设备

	global_input_device->name = pdev->name;
	global_input_device->phys = pdev->name;
	global_input_device->dev.parent = dev;

	global_input_device->id.bustype = BUS_HOST;
	global_input_device->id.vendor = 0x0001;
	global_input_device->id.product = 0x0001;
	global_input_device->id.version = 0x0100;

    // set 1 设置哪一类型事件
    __set_bit(EV_KEY, global_input_device->evbit);// 设置 按键 
    __set_bit(EV_ABS, global_input_device->evbit);// 绝对位移

    // set 2 设置哪些事件 
    // /include/uapi/linux/input-event-codes.h
    __set_bit(BIN_TOUCH, global_input_device->keybit);// 对于 触摸屏来说 支持哪一个 按键 BIN_TOUCH
    __set_bit(ABS_MT_SLOT, global_input_device->keybit);// 触摸屏 支持的 绝对位移 是 具体是哪一个  
    __set_bit(ABS_MT_POSITION_X, global_input_device->keybit);
    __set_bit(ABS_MT_POSITION_Y, global_input_device->keybit);


    // set 3 设置 参数 ，例如 最大值 最小值
    input_set_abs_params(global_input_device, ABS_MT_POSITION_X, 0, 0xFFFF, 0, 0);// 设置 绝对位移 的参数



    // register 设置完成 后 开始 注册

    err = input_register_device(global_input_device);


    /** 硬件相关操作 */
    //从 平台设备 platform device 中获取 中断号
    irq =  platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (!irq) {
        dev_err(dev, "Failed to get IRQ resource\n");
        return -EINVAL;
    }
    // 获取中断号

    // err = devm_request_irq(dev, irq->start, input_device_irq_handler, IRQF_TRIGGER_RISING, "input_device_irq", NULL);
    err = devm_request_irq(dev, irq->start, input_device_irq_handler, irq->flags, "input_device_irq", NULL);

    return 0;
}

static int input_device_remove(struct platform_device *pdev)
{

    struct resource *irq =  platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    // 释放中断
    free_irq(irq, NULL);
    // 注销设备
    input_unregister_device(global_input_device);
    return 0;
}

static void input_device_shutdown(struct platform_device *pdev)
{

}

 static const struct of_device_id input_device_of_match[] = {
    { .compatible = "dirve,input_device", },
    { },
};


static struct platform_driver input_device_device_driver = {
    .probe      = input_device_probe,
    .shutdown   = input_device_shutdown,
    .remove     = input_device_remove,
    .driver     = {
        .name   = "input_device",
        .of_match_table = input_device_of_match,
    }
};



static int __init input_device_init(void)
{
    return platform_driver_register(&input_device_device_driver);
}

static void __exit input_device_exit(void)
{
    platform_driver_unregister(&input_device_device_driver);
}

late_initcall(input_device_init);
module_exit(input_device_exit);

MODULE_LICENSE("GPL");


