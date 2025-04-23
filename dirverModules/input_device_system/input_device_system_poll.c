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


#define DEVICE_NAME "my_input_device"
#define CLASS_NAME "my_input_device_class"
#define BUFFER_LEN 128
// static struct input_dev *global_input_device;
// static int input_device_irq;

struct input_device_button_data {
    struct input_dev *input_device; //  输入设备
    struct gpio_desc * gpiod;// GPIO描述符
    int irq;// 中断号
    unsigned int key_code;// 按键码
    unsigned long press_count; /* 按键按下次数 */
};

struct input_device_data {
    struct input_dev *input_device;
    int n_buttons;
    struct input_device_button_data button_data[];
    // struct input_device_button_data *button_data;
};





/* 环形缓冲区 */

struct button_event {
    unsigned int button_idx;// 按键索引
    struct input_event input_ev;   /* 输入事件：type, code, value, time */
    unsigned long press_count;     /* 按下次数 */
}


// static struct button_event event_buffer[BUFFER_LEN];
// static DEFINE_SPINLOCK(buffer_lock); // 环形缓冲区的锁 ; 
static int read_index = 0;
static int write_index = 0;




struct my_input_device{
    struct input_device_data *input_data;
    struct cdev cdev;// 字符设备
    dev_t dev_no;// 设备号
    struct class *class;//储存  class_create创建的 类 的返回值
    struct device *device;// 储存  device_create 的返回值

    struct button_event event_buffer[BUFFER_LEN];
    spinlock_t buffer_lock;
    int read_index = 0;
    int write_index = 0;
    wait_queue_head_t wait_queue;// 等待队列
}
/* 全局设备数据 */
static struct my_input_device *my_input_dev;



/* 环形缓冲区操作 */
static int next_pos(int pos)
{
    return (pos + 1) % BUFFER_LEN;
}

static int is_buffer_empty(struct my_input_device *my_input_dev)
{
    // return (read_index == write_index);
    return (my_input_dev->read_index == my_input_dev->write_index);
}

static int is_buffer_full(struct my_input_device *my_input_dev)
{
    // return (read_index == next_pos(write_index));
    return (my_input_dev->read_index == my_input_dev->next_pos(my_input_dev->write_index));
}


static void push_event(struct my_input_device *my_input_dev, struct button_event *event){
    // unsigned long flags;
    // spin_lock_irqsave(&buffer_lock, flags);// 获取锁
    // if (!is_buffer_full()) {
    //     event_buffer[write_index] = *event;
    //     write_index = next_pos(write_index);
    // }
    // spin_unlock_irqrestore(&buffer_lock, flags);// 释放锁


    unsigned long flags;
    int was_buffer_empty;
    spin_lock_irqsave(&my_input_dev->buffer_lock, flags);// 获取锁
    was_buffer_empty = is_buffer_empty(my_input_dev);
    if(!is_buffer_full(my_input_dev)){
        my_input_dev->event_buffer[my_input_dev->write_index] = *event;
        my_input_dev->write_index = next_pos(my_input_dev->write_index);
        if(was_buffer_empty){
            wake_up_interruptible(&my_input_dev->wait_queue);// 唤醒等待队列
        }
    }
    spin_unlock_irqrestore(&my_input_dev->buffer_lock, flags);// 释放锁
}

static int get_event(struct my_input_device *my_input_dev, struct button_event *event){
    // unsigned long flags;
    // spin_lock_irqsave(&buffer_lock, flags);// 获取锁
    // int ret = 0;
    // if (!is_buffer_empty()) {
    //     *event = event_buffer[read_index];
    //     read_index = next_pos(read_index);
    //     ret = 1;
    // }
    // spin_unlock_irqrestore(&buffer_lock, flags);// 释放锁
    // return ret;


    unsigned long flags;
    int ret = 0;
    spin_lock_irqsave(&my_input_dev->buffer_lock, flags);// 获取锁

    if(!is_buffer_full(my_input_dev)){
        *event = my_input_dev->event_buffer[my_input_dev->read_index];
        my_input_dev->read_index = next_pos(my_input_dev->read_index);
        ret = 1;
    }
    spin_unlock_irqrestore(&my_input_dev->buffer_lock, flags);// 释放锁
    return ret;
}










/* 中断处理函数 */
// static irqreturn_t input_device_irq_handler(int irq, void *dev_id){
//     /** 读取硬件 得到数据 */



//     /** 上报数据  input_event */
//     // input_event(global_input_device, EV_KEY, XX, 0);// 上报 按键事件
//     // input_sync(global_input_device);// 同步事件

//     struct input_device_button_data *button_data = dev_id;

//     int state = gpiod_get_value(button_data->gpiod);
//     if (state < 0) {
//         dev_err(&button_data->input_device->dev, "Failed to read GPIO: %d\n", state);
//         return IRQ_HANDLED;
//     }


//     // 上报按键事件
//     input_event(button_data->input_device, EV_KEY, button_data->key_code, state);
//     input_sync(button_data->input_device);


//     // printk(KERN_INFO "input_device_irq_handler irq %d\n", irq);
//     dev_dbg(&button_data->input_device->dev, "IRQ %d triggered, key_code=%d, state=%d\n",
//         irq, button_data->key_code, state);
//     return IRQ_HANDLED;
// }

static irqreturn_t input_device_irq_handler(int irq, void *dev_id)
{
    struct input_device_button_data *button_data = dev_id;
    static unsigned long last_time;
    unsigned long now = jiffies;
    int state;
    struct button_event event;
    struct my_input_device *dev = my_input_dev;

    /* 忽略 50ms 内的重复中断 */
    if (time_before(now, last_time + msecs_to_jiffies(50)))
        return IRQ_HANDLED;
    last_time = now;

    state = gpiod_get_value(button_data->gpiod);// 读取GPIO状态
    if (state < 0) {
        dev_err(&button_data->input_device->dev, "Failed to read GPIO: %d\n", state);
        return IRQ_HANDLED;
    }



    // 中断的时候 是 按键 按下的时候 把 数据 推进 缓冲区
    if (state == 1){
        button_data->press_count++;
    }
    
    // 计算 button_data（当前按键）相对于 button_data[] 数组首地址的偏移，得到按键索引（button_idx）。
    //  例如，button_data = &button_data[2] → button_idx = 2。
    // 逻辑：
    //  button_data：中断触发的按键描述符（例如 &button_data[2]）。
    //  button_data->input_device->dev.driver_data：指向 input_data。
    //  driver_data->button_data：数组首地址（&button_data[0]）。
    //  指针减法：&button_data[2] - &button_data[0] = 2。

    //收到的数据放到 缓冲区
    event.button_idx = button_data - button_data->input_device->dev.driver_data->button_data;// 计算按键索引
    event.input_ev.type = EV_KEY;
    event.input_ev.code = button_data->key_code;
    event.input_ev.value = state;
    // event.input_ev.time = ktime_get();
    getnstimeofday(&event.input_ev.time);
    event.press_count = button_data->press_count;
    push_event(dev, &event);




    input_event(button_data->input_device, EV_KEY, button_data->key_code, state);
    input_sync(button_data->input_device);
    dev_dbg(&button_data->input_device->dev, "IRQ %d triggered, key_code=%d, state=%d\n", irq, button_data->key_code, state);

    dev_info(&button_data->input_device->dev, "IRQ %d triggered, key_code=%d, state=%d\n", irq, button_data->key_code, state);
    return IRQ_HANDLED;
}



static int my_input_open(struct inode *inode, struct file *file){
    file->private_data = my_input_dev;
    return 0;
}

static ssize_t my_input_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    struct my_input_device *dev = file->private_data;
    struct button_event event;
    char tmp[256];
    int ret = 0;

    // 这段 拿到了 event
    if(!get_event(dev, &event)){
        return -EAGAIN;
    }

    // 这段 把 event 写到 用户空间
    ret = snprintf(tmp, sizeof(tmp),
                   "Button %u: type=%u, code=%u, value=%d, press_count=%lu, time=%ld.%06ld\n",
                   event.button_idx, event.input_ev.type, event.input_ev.code,
                   event.input_ev.value, event.press_count,
                   event.input_ev.time.tv_sec, event.input_ev.time.tv_usec);
    if (len > count){
        len = count;
    }
    if (copy_to_user(buf, tmp, len)){
        return -EFAULT;
    }

    return ret;
}


static ssize_t my_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{
    
    // printk(KERN_INFO "%s: Write operation not supported\n", DEVICE_NAME);
    return -EINVAL;
}

static int my_input_release(struct inode *inode, struct file *file)
{
    return 0;
}

static unsigned int my_poll(struct file *file, struct poll_table_struct *wait)
{
    struct my_input_device *dev = file->private_data;
    // printk(KERN_INFO "%s: Poll operation not supported\n", DEVICE_NAME);
    // poll_wait(file, &my_wait_queue, wait);//  这里会休眠不会休眠

    poll_wait(file, &dev->wait_queue, wait);//  这里会休眠不会休眠


    // 判断是否为 空 为 空 返回 0, 否则返回 POLLIN | POLLRDNORM； POLLIN | POLLRDNORM 表示有数据可读
    return is_int_buffer_empty() ? 0 : POLLIN | POLLRDNORM;
    // return 0;
}


// ioctl 是 用户空间 和 内核空间 通信 的一个接口
static long my_input_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct my_input_device *dev = file->private_data;


    return -ENOTTY;
}


static unsigned int my_input_poll(struct file *file, struct poll_table_struct *wait)
{
    struct my_input_device *dev = file->private_data;

    // poll_wait 函数将文件对象 file 和等待队列 my_wait_queue 注册到 poll_table_struct 结构体 wait 中，以便在等待队列上等待事件发生。

    // 判断是否为 空 为 空 返回 0, 否则返回 POLLIN | POLLRDNORM； POLLIN | POLLRDNORM 表示有数据可读
    if(!is_buffer_empty()){
        return POLLIN | POLLRDNORM;
    }

    return 0;
}


static const struct file_operations input_device_fops = {
    .owner = THIS_MODULE,
    .open = my_input_open,
    .read = my_input_read,
    .write = my_input_write,
    .release = my_input_release,
    .poll = my_input_poll,
    .unlocked_ioctl = my_input_ioctl,// ioctl 是 用户空间 和 内核空间 通信 的一个接口
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

    //  分配字符设备 =====================================================================
    /** 分配字符设备 */
    my_input_dev = devm_kzalloc(dev, sizeof(*my_input_dev), GFP_KERNEL);
    if (!my_input_dev){
        dev_err(dev, "Failed to allocate my_input_dev\n");
        return -ENOMEM;
    }
    my_input_dev->input_data = input_device_data;


    // 初始化自旋锁  
    spin_lock_init(&my_input_dev->buffer_lock);
    my_input_dev->read_inx = 0;// 初始化读索引
    my_input_dev->write_inx = 0;// 初始化写索引
    // 初始化等待队列头
    init_waitqueue_head(&my_input_dev->wait_queue);// 

    

    // 初始化字符设备 结构体， 跟之前 的 register_chrdev 有些不太一样
    err = alloc_chrdev_region(&my_input_dev->dev_no, 0, 1, DEVICE_NAME);// 分配设备号, 存放在 my_input_dev->dev_no 中 
    if (err < 0) {
        dev_err(dev, "Failed to allocate device number\n");
        return err;
    }

    // 注册字符设备 以前是  register_chrdev(0, DEVICE_NAME, &my_fops);
    cdev_init(&my_input_dev->cdev, &input_device_fops);// 初始化字符设备 使用 cdev_init 函数， 将字符设备结构体和文件操作结构体关联起来
    my_input_dev->cdev.owner = THIS_MODULE;// 设置字符设备所有者; THIS_MODULE 是 宏定义，表示当前模块

    err = cdev_add(&my_input_dev->cdev, my_input_dev->dev_no, 1);// 添加字符设备;  dev_add 将初始化的 cdev 注册到内核，关联设备号 my_input_dev->dev_no 和设备数量
    if (err) {
        dev_err(dev, "Failed to add cdev: %d\n", err);
        unregister_chrdev_region(my_input_dev->dev_no, 1);// 释放设备号
        return err;
    }

    // 创建设备类 和 节点
    my_input_dev->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(my_input_dev->class)) {
        dev_err(dev, "Failed to create class\n");
        cdev_del(&my_input_dev->cdev);// 删除字符设备
        unregister_chrdev_region(my_input_dev->dev_no, 1);// 释放设备号
        return PTR_ERR(my_input_dev->class);
    }

    // char device_name_buf[30];
    // int minor = 0;
    // snprintf(device_name_buf, sizeof(device_name_buf), "%s_%d", DEVICE_NAME, minor);
    // 创建设备节点
    my_input_dev->device = device_create(
        my_input_dev->class,
        dev,
        my_input_dev->dev_no,
        NULL,
        DEVICE_NAME
    );

    //  分配字符设备 end =======================================




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

        // 设置去抖时间 需要设备树上有 debounce-interval




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


    device_destroy(my_input_dev->class, my_input_dev->dev_no);// 销毁设备
    class_destroy(my_input_dev->class);// 销毁类
    cdev_del(&my_input_dev->cdev);// 删除字符设备
    unregister_chrdev_region(&my_input_dev->dev_no, 1);// 释放设备号
    kfree(my_input_dev);// 释放内存


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




