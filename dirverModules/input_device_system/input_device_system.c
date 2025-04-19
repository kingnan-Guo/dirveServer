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
#include <dt-bindings/input/gpio-keys.h>

// static struct input_dev *global_input_device;
// static int input_device_irq;

struct input_device_button_data {
    struct input_dev *input_device; //  输入设备
    struct gpio_desc * gpiod;// GPIO描述符
    int irq;// 中断号
    unsigned int key_code;// 按键码
};

struct input_device_data {
    struct input_dev *input_device;
    int n_buttons;
    struct input_device_button_data button_data[];
    // struct input_device_button_data *button_data;
};


/* 中断处理函数 */
static irqreturn_t input_device_irq_handler(int irq, void *dev_id){
    /** 读取硬件 得到数据 */



    /** 上报数据  input_event */
    // input_event(global_input_device, EV_KEY, XX, 0);// 上报 按键事件
    // input_sync(global_input_device);// 同步事件

    struct input_device_button_data *button_data = dev_id;

    int state = gpiod_get_value(button_data->gpiod);
    if (state < 0) {
        dev_err(&button_data->input_device->dev, "Failed to read GPIO: %d\n", state);
        return IRQ_HANDLED;
    }


    // 上报按键事件
    input_event(button_data->input_device, EV_KEY, button_data->key_code, state);
    input_sync(button_data->input_device);


    // printk(KERN_INFO "input_device_irq_handler irq %d\n", irq);
    dev_dbg(&button_data->input_device->dev, "IRQ %d triggered, key_code=%d, state=%d\n",
        irq, button_data->key_code, state);
    return IRQ_HANDLED;
}


static int input_device_probe(struct platform_device *pdev)
{
    int err;
    // struct resource *irq;
    struct device *dev = &pdev->dev;
    struct input_device_data *input_device_data;
    struct device_node *node, *child;


    // input_device_data = devm_kzalloc(dev, sizeof(*input_device_data), GFP_KERNEL);

    // 获取子节点的数量
    int nbuttons = device_get_child_node_count(dev);
    // printk(KERN_INFO "nbuttons = %d\n", nbuttons);
    dev_info(dev, "nbuttons = %d\n", nbuttons);

    // 分配私有数据 
    // 这是 分配 一个 input_device_data 大小 + 数组 大小的 内存
    // input_device_data = devm_kzalloc(dev, sizeof(*input_device_data) + nbuttons  * sizeof(input_device_data->button_data[0]), GFP_KERNEL);
    input_device_data = devm_kzalloc(dev, struct_size(input_device_data, button_data, nbuttons), GFP_KERNEL);// 跟上面是相同的意思
   
    // 储存 子节点数量
    input_device_data->n_buttons = nbuttons;







    /** 从设备树里面 获得 设备信息 */

    /** / alloc / set / register       struct input_device */
    /** 分配    / 设置 / 注册            struct input_device */

    input_device_data->input_device = devm_input_allocate_device(dev);// 分配输入设备
    if (!input_device_data->input_device) {
        dev_err(dev, "Failed to allocate input device\n");
        return -ENOMEM;
    }


    input_device_data->input_device->name = "input_device";
    input_device_data->input_device->phys = "input_device";
    input_device_data->input_device->dev.parent = dev;
    input_device_data->input_device->id.bustype = BUS_HOST;
    input_device_data->input_device->id.vendor = 0x0001;
    input_device_data->input_device->id.product = 0x0001;
    input_device_data->input_device->id.version = 0x0100;



	// global_input_device->name = pdev->name;
	// global_input_device->phys = pdev->name;
	// global_input_device->dev.parent = dev;

	// global_input_device->id.bustype = BUS_HOST;
	// global_input_device->id.vendor = 0x0001;
	// global_input_device->id.product = 0x0001;
	// global_input_device->id.version = 0x0100;
    // input_device_data->input_device = devm_input_allocate_device(dev);// 分配输入设备;
    // input_device_data->gpiod = devm_gpiod_get(dev, NULL, GPIOD_IN);// 



   // set 1 设置哪一类型事件
    __set_bit(EV_KEY, input_device_data->input_device->evbit);// 设置 按键
    

    /* 遍历子节点，设置每个按键 */
    node = dev->of_node;
    int  i = 0;
    for_each_child_of_node(node, child){

        input_device_data->button_data[i].input_device = input_device_data->input_device;



        // &child->fwnode 表示设备树中子节点（button@0, button@1）的固件节点句柄，用于：
        // 获取 GPIO：devm_fwnode_gpiod_get(dev, &child->fwnode, ...)
        // 读取按键码：fwnode_property_read_u32(&child->fwnode, "linux,code", ...)
        // 获取中断号：fwnode_irq_get(&child->fwnode, 0)

        input_device_data->button_data[i].gpiod = devm_fwnode_gpiod_get(dev, &child->fwnode, NULL, GPIOD_IN, "button");// 获取 GPIO 描述符
        if (IS_ERR(input_device_data->button_data[i].gpiod)) {
            err = PTR_ERR(input_device_data->button_data[i].gpiod);
            dev_err(dev, "[devm_fwnode_gpiod_get] Failed to get GPIO for %s: %d\n", child->name, err);
            of_node_put(child);
            return err;
        }
        
        
        
        // u32 key_code;// 按键码
        // err = fwnode_property_read_u32(child, "linux,code", &key_code);// 获取按键码
        // err = fwnode_property_read_u32(child, "linux,code", input_device_data->button_data[i]->key_code);// 获取按键码

        err = fwnode_property_read_u32(&child->fwnode, "linux,code", &input_device_data->button_data[i].key_code);
        if (err) {
            dev_err(dev, "Missing linux,code for %s\n", child->name);
            of_node_put(child);
            return err;
        }




        // // set 2 设置哪些事件 
        // 设置输入设备 支持的按键； 因为 上面设置了 支持 EV_KEY  ，所以这里 设置 按键码
        __set_bit(input_device_data->button_data[i].key_code, input_device_data->input_device->keybit);// 设置按键码
        
        
        // 获取中断号
        input_device_data->button_data[i].irq = fwnode_irq_get(&child->fwnode, 0);
        if (input_device_data->button_data[i].irq < 0) {
            dev_err(dev, "[fwnode_irq_get] Failed to get IRQ for %s: %d\n", child->name, input_device_data->button_data[i].irq);
            of_node_put(child);
            return input_device_data->button_data[i].irq;
        }

        // 注册中断
        // err = devm_request_irq(
        //     dev,
        //     input_device_data->button_data[i]->irq,
        //     input_device_irq_handler,
        //     IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, // 中断触发方式
        //     child->name,
        //     input_device_data->button_data[i] // 传递给中断处理函数 gpio_key_isr 的参数
        // );

        /** 获取触发类型 */
        unsigned int trigger_type = irq_get_trigger_type(input_device_data->button_data[i].irq);
        if (!trigger_type){
            trigger_type = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;
        }
        
        err = devm_request_irq(
            dev, 
            input_device_data->button_data[i].irq, 
            input_device_irq_handler, 
            trigger_type, 
            child->name, 
            &input_device_data->button_data[i]  // 传递给中断处理函数 gpio_key_isr 的参数
        );


        if (err) {
            dev_err(dev, "[devm_request_irq]Failed to request IRQ %d for %s: %d\n", input_device_data->button_data[i].irq, child->name, err);
            of_node_put(child);
            return err;
        }


        dev_info(dev, "Added button %s: GPIO %d, IRQ %d, key_code %d\n",
            child->name, desc_to_gpio(input_device_data->button_data[i].gpiod),
            input_device_data->button_data[i].irq, input_device_data->button_data[i].key_code);

        i++;

    }


    // 3 register 设置完成 后 开始 注册
    // 注册 输入设备 
    err = input_register_device(input_device_data->input_device);
    if (err) {
        // printk(KERN_ERR "[input_register_device] input_device: Failed to register input device: %d\n", err);
        dev_err(dev, "[input_register_device]  Failed to register input device: %d\n", err);
        return err;
    }
    
    /**
     * platform_set_drvdata 的作用
     *  将私有数据（input_device_data）与平台设备（pdev）绑定，存储在 pdev->dev.driver_data。
     *  允许在驱动的其他函数（remove, shutdown）通过 platform_get_drvdata 访问
     * 
     * 在 probe 中绑定 input_device_data，包含 input_device, n_buttons, button_data
     * 为未来扩展（例如在 remove 中访问 input_device_data）提供支持
     */
    platform_set_drvdata(pdev, input_device_data);

    dev_info(dev, "Input device probed successfully\n");


    // // set 1 设置哪一类型事件
    // __set_bit(EV_KEY, global_input_device->evbit);// 设置 按键 
    // __set_bit(EV_ABS, global_input_device->evbit);// 绝对位移

    // // set 2 设置哪些事件 
    // // /include/uapi/linux/input-event-codes.h
    // __set_bit(BTN_TOUCH, global_input_device->keybit);// 对于 触摸屏来说 支持哪一个 按键 BIN_TOUCH
    // __set_bit(ABS_MT_SLOT, global_input_device->keybit);// 触摸屏 支持的 绝对位移 是 具体是哪一个  
    // __set_bit(ABS_MT_POSITION_X, global_input_device->keybit);
    // __set_bit(ABS_MT_POSITION_Y, global_input_device->keybit);


    // // set 3 设置 参数 ，例如 最大值 最小值
    // input_set_abs_params(global_input_device, ABS_MT_POSITION_X, 0, 0xFFFF, 0, 0);// 设置 绝对位移 的参数



    // // register 设置完成 后 开始 注册

    // err = input_register_device(global_input_device);


    // /** 硬件相关操作 */
    // //从 平台设备 platform device 中获取 中断号
    // irq =  platform_get_resource(pdev, IORESOURCE_IRQ, 0); // platform_get_resource 只能获取  input_device 父节点的 中断 资源
    // if (!irq) {
    //     dev_err(dev, "Failed to get IRQ resource\n");
    //     return -EINVAL;
    // }
    // // 获取中断号

    // // err = devm_request_irq(dev, irq->start, input_device_irq_handler, IRQF_TRIGGER_RISING, "input_device_irq", NULL);
    // err = devm_request_irq(dev, irq->start, input_device_irq_handler, irq->flags, "input_device_irq", NULL);

    return 0;
}

static int input_device_remove(struct platform_device *pdev)
{
    struct input_device_data *data = platform_get_drvdata(pdev);
    dev_info(&pdev->dev, "Removed, n_buttons=%d\n", data->n_buttons);
    // struct resource *irq =  platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    // // 释放中断
    // free_irq(irq, NULL);
    // // 注销设备
    // input_unregister_device(global_input_device);
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

// late_initcall(input_device_init);// 在所有其他模块初始化之后调用
module_init(input_device_init);
module_exit(input_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Multi-button input device driver");




