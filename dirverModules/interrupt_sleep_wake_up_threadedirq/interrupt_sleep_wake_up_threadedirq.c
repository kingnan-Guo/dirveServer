#include <linux/module.h>
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
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/gpio/consumer.h>
#include <linux/poll.h>
#include <linux/of_irq.h>
#include <linux/fcntl.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <asm/current.h>


#define DEVICE_NAME "my_gpio_threadedirq"
 
static int major;
static struct class *gpio_class;

struct gpio_key {
    int gpio;
    struct gpio_desc *desc; // GPIO 描述符
    int irq;                // 中断号
    int index;              // GPIO 索引
    struct timer_list key_timer;// 定时器
    struct tasklet_struct key_tasklet;// tasklet 结构体
    struct work_struct key_work;// work 结构体 工作队列
};

static struct gpio_key *gpio_keys;
static int gpio_count;

static int global_key = 0;// 用在 

DECLARE_WAIT_QUEUE_HEAD(my_wait_queue);


// fasync 结构体
static struct fasync_struct *my_async_queue;


/**
 * 环形缓冲区
 *  
 * 
 */

// static const int BUFFER_LEN = 128;
#define BUFFER_LEN 128
static int global_int_buffer[BUFFER_LEN];
static int read_index = 0;
static int write_index = 0;

// 指针的下一个位置
static int next_pos(int pos){
    return (pos + 1) % BUFFER_LEN;
}

// 当 读取指针 等于 写入指针 时, 缓冲区为空
static int is_int_buffer_empty(void){
    return (read_index == write_index);
}

// 当 读取指针 等于 写入指针 的下一个位置 时, 缓冲区为满
static int is_int_buffer_full(void){
    return (read_index == next_pos(write_index));
}
static void push_int_buffer(int value){
    // 如果不是满的 就可以写入
    if(!is_int_buffer_full()){
        // 写入数据
        global_int_buffer[write_index] = value;
        // 写入指针 后移
        write_index = next_pos(write_index);
    }
}

// 从缓冲区中读取数据
static int get_int_buffer(void){
    int value = 0;
    if(!is_int_buffer_empty()){
        // 读取数据
        value = global_int_buffer[read_index];
        // 读取指针后移
        read_index = next_pos(read_index);
    }
    return value;
}


/* 文件操作函数 */
static int my_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "%s: Device opened\n", DEVICE_NAME);
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "%s: Write operation not supported\n", DEVICE_NAME);

    int err;

    if(is_int_buffer_empty() && (file->f_flags & O_NONBLOCK)){
        return -EAGAIN;
    }


    // wait_event_interruptible(my_wait_queue, global_key);
    wait_event_interruptible(my_wait_queue, !is_int_buffer_empty());

    global_key = get_int_buffer();

    // 将 global_key 写入用户空间
    err = copy_to_user(buffer, &global_key, sizeof(global_key));
    if (err != 0) {
        printk(KERN_INFO "%s: Error copying data to user space\n", DEVICE_NAME);
        return -EFAULT;
    }
    printk(KERN_INFO "%s: global_key = %d\n", DEVICE_NAME, global_key);
    // buffer
    printk(KERN_INFO ": buffer  %s \n", buffer);
    global_key = 0;
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "%s: Write operation not supported\n", DEVICE_NAME);
    return -EINVAL;
}

static int my_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "%s: Device closed\n", DEVICE_NAME);
    return 0;
}

static unsigned int my_poll(struct file *file, struct poll_table_struct *wait)
{
    printk(KERN_INFO "%s: Poll operation not supported\n", DEVICE_NAME);
    poll_wait(file, &my_wait_queue, wait);//  这里会休眠不会休眠
    // 判断是否为 空 为 空 返回 0, 否则返回 POLLIN | POLLRDNORM； POLLIN | POLLRDNORM 表示有数据可读
    return is_int_buffer_empty() ? 0 : POLLIN | POLLRDNORM;
}

static int my_drv_fasync(int fd, struct file *file, int on){
    printk(KERN_INFO "%s: fasync operation not supported\n", DEVICE_NAME);

    // 这里 要调用 fasync_helper 函数
    // fasync_helper 会在 my_async_queue 中 fa_file 指向 file ， file 包含  pid
    if(fasync_helper(fd, file, on, &my_async_queue)){
        return 0;
    }
    return -EINVAL;
}

// my_timer_expire
static void my_timer_expire(struct timer_list *timer){
    // 通过当前的 定时起 找到  gpio_key， 有 from_timer 函数
    struct gpio_key *key = from_timer(key, timer, key_timer);


    int value = gpiod_get_value(key->desc);// 获取 GPIO 的值
    printk(KERN_INFO "%s: my_timer_expire on GPIO %d , value = %d\n", DEVICE_NAME, key->index, value);
    
   // global_key = (key->gpio << 8) | value;// gpio 的编号 和 值 通过或运算 合并, gpio编号 占 高 8 位, 值 占低 8 位
    global_key = (key->gpio << 8) | value;
    // 分别打印 (key->gpio << 8)  和 value
    printk(KERN_INFO "%s: key->gpio = %d\n", DEVICE_NAME, key->gpio);
    printk(KERN_INFO "%s: value = %d\n", DEVICE_NAME, value);
    printk(KERN_INFO "%s: global_key = %d\n", DEVICE_NAME, global_key);
    
    // 将 global_key 写入 环形缓冲区
    push_int_buffer(global_key);
    
    wake_up_interruptible(&my_wait_queue);

    // 通知 fasync 队列
    kill_fasync(&my_async_queue, SIGIO, POLL_IN);// 通知 fasync 队列


}

// tasklet 函数; data 是 tasklet 的参数
static void my_tasklet_func(unsigned long data){
    struct gpio_key *key = (struct gpio_key *)data;

    int value = gpiod_get_value(key->desc);// 获取 GPIO 的值
    

    printk(KERN_INFO "%s:my_tasklet_func  on GPIO %d , value = %d\n", DEVICE_NAME, key->gpio, value);

}

// work 函数 my_work_func
static void my_work_func(struct work_struct *work){
    struct gpio_key *key = container_of(work, struct gpio_key, key_work);// 通过 work 找到 gpio_key
    int value = gpiod_get_value(key->desc);// 获取 GPIO 的值
    printk(KERN_INFO "%s: my_work_func  on GPIO %d , value = %d\n", DEVICE_NAME, key->gpio, value);
    printk(KERN_INFO "%s: my_work_func the process is %s pid %d\n", DEVICE_NAME,current->comm, current->pid);
}


static const struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_read,
    .write = my_write,
    .release = my_release,
    .poll = my_poll,
    .fasync = my_drv_fasync,
};





/* 中断处理函数 */
static irqreturn_t gpio_key_isr(int irq, void *dev_id)
{
    struct gpio_key *key = dev_id;// 获取中断的 GPIO; dev_i是  devm_request_irq 传过来的值
    printk("gpio_key_isr key %d irq \n", key->gpio);
    // int value = gpiod_get_value(key->desc);// 获取 GPIO 的值
    // printk(KERN_INFO "%s: Interrupt on GPIO %d (IRQ %d), value = %d\n", DEVICE_NAME, key->index, irq, value);
    
    // // global_key = (key->gpio << 8) | value;// gpio 的编号 和 值 通过或运算 合并, gpio编号 占 高 8 位, 值 占低 8 位
    // global_key = (key->gpio << 8) | value;
    // // 分别打印 (key->gpio << 8)  和 value
    // printk(KERN_INFO "%s: key->gpio = %d\n", DEVICE_NAME, key->gpio);
    // printk(KERN_INFO "%s: value = %d\n", DEVICE_NAME, value);
    // printk(KERN_INFO "%s: global_key = %d\n", DEVICE_NAME, global_key);
    
    // // 将 global_key 写入 环形缓冲区
    // push_int_buffer(global_key);

    
    // wake_up_interruptible(&my_wait_queue);

    // // 通知 fasync 队列
    // kill_fasync(&my_async_queue, SIGIO, POLL_IN);// 通知 fasync 队列

    // // 每次发生中断 那么 会 重新设置定时器




    // 设置 tasklet
    tasklet_schedule(&key->key_tasklet);// 调度 tasklet


    //设置定时器的 时间
    // mod_timer(&key->key_timer, jiffies + msecs_to_jiffies(1000));// 设置定时器
    mod_timer(&key->key_timer, jiffies + HZ / 50);// 设置定时器 HZ / 50 = 20ms


    // 调度 工作队列
    schedule_work(&key->key_work);
    
    
    return IRQ_HANDLED;
}

/* 平台设备探测函数 */
static int my_gpio_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct device_node *node = dev->of_node;
    int err, i;

    printk(KERN_INFO "%s: Probing device\n", DEVICE_NAME);

    // gpio_count = of_count_phandle_with_args(node, "interrupt-gpios", "#gpio-cells");

    gpio_count = gpiod_count(dev, NULL);
    if (gpio_count <= 0) {
        dev_err(dev, "No GPIOs found in device tree, count = %d\n", gpio_count);
        return -EINVAL;
    }
    printk(KERN_INFO "%s: Found %d GPIOs\n", DEVICE_NAME, gpio_count);

    if (!of_find_property(node, "gpios", NULL)) {
        dev_err(dev, "No 'gpios' property in device tree!\n");
        return -EINVAL;
    }
    if (!of_find_property(node, "pinctrl-0", NULL)) {
        dev_err(dev, "No 'pinctrl-0' property in device tree!\n");
        return -EINVAL;
    }


    // gpio_keys = kzalloc(sizeof(struct gpio_key) * gpio_count, GFP_KERNEL);
    // 设备 卸载的时候 自动清除
    gpio_keys = devm_kzalloc(dev, sizeof(struct gpio_key) * gpio_count, GFP_KERNEL);
    if (!gpio_keys) {
        dev_err(dev, "Failed to allocate memory for gpio_keys\n");
        return -ENOMEM;
    }


    for (i = 0; i < gpio_count; i++)
    {
        unsigned int gpio_num;
        struct gpio_desc *desc;
        // 获取 gpio 的编号
        // gpio_num = gpiod_get(node, i);
        gpio_num = of_get_named_gpio(node, "gpios", i);// 获取 gpio 的编号
        if (gpio_num < 0) {
            dev_err(dev, "Failed to get GPIO %d (num %u): %d\n", i, gpio_num, gpio_num);
        }
        printk(KERN_INFO "%s: GPIO %d parsed as %u\n", DEVICE_NAME, i, gpio_num);
        

        desc = gpiod_get_index(dev, NULL, i, GPIOD_IN);
        if (IS_ERR(desc)) {
            err = PTR_ERR(desc);
            dev_err(dev, "Failed to get GPIO %d (num %u): %d\n", i, gpio_num, err);
            goto free_gpios;
        }

        int irq = gpiod_to_irq(desc); // 获取中断号
        if (irq < 0) {
            dev_err(dev, "Failed to get IRQ for GPIO %d: %d\n", i, irq);
            gpiod_put(desc);
            err = irq;
            goto free_gpios;
        }

        
        // gpio_num = gpiod_to_chip(desc);
        // if (gpio_num < 0) {
        //     // 错误处理，可能是无效的描述符
        //     pr_err("Failed to get GPIO number\n");
        // } else {
        //     pr_info("GPIO number: %d\n", gpio_num);
        // }

        gpio_keys[i].desc = desc;
        gpio_keys[i].irq = irq;
        gpio_keys[i].index = i;
        gpio_keys[i].gpio = gpio_num;


        // 初始化定时器
        timer_setup(&gpio_keys[i].key_timer, my_timer_expire, 0);// 初始化定时器
        gpio_keys[i].key_timer.expires = ~0; // 设置定时器的时间; ~0 表示定时器不运行
        add_timer(&gpio_keys[i].key_timer);// 添加定时器


        // 初始化 tasklet
        tasklet_init(&gpio_keys[i].key_tasklet, my_tasklet_func, (unsigned long)&gpio_keys[i]);// 初始化 tasklet
         

        // 初始化 工作队列
        INIT_WORK(&gpio_keys[i].key_work, my_work_func);


        err = devm_request_irq(
            dev,
            irq,
            gpio_key_isr,
            IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, // 设置中断触发方式
            "my_gpio_poll", // 中断名字
            &gpio_keys[i] // 传递给中断处理函数 gpio_key_isr 的参数
        );

        if (err) {
            dev_err(dev, "Failed to request IRQ %d for GPIO %d: %d\n", irq, i, err);
            gpiod_put(desc);
            goto free_gpios;
        }


        dev_info(dev, "Registered IRQ %d for GPIO %d\n", irq, i);

    }
    





    // 2、 注册  file_operations _fops
    major = register_chrdev(0, DEVICE_NAME, &my_fops);
    if (major < 0) {
        dev_err(dev, "Failed to register character device: %d\n", major);
        err = major;
        goto free_gpios;
        return major;
    }

    gpio_class = class_create("my_gpio_threadedirq_class");
    if(IS_ERR(gpio_class)){
        printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        dev_err(dev, "Failed to create class: %d\n", err);
		unregister_chrdev(major, DEVICE_NAME);
        // printk("failed to allocate class\n");
        // return PTR_ERR(gpio_class);
    }



    char device_name_buf[30];
    int minor = 0;
    snprintf(device_name_buf, sizeof(device_name_buf), "%s_%d", DEVICE_NAME, minor);
    device_create(gpio_class, NULL, MKDEV(major, minor), NULL, device_name_buf);//创建 文件系统 的设备节点; 应用程序 通过文件系统的设备 节点 访问 硬件  
    // device_create(_class, NULL, MKDEV(major, 0), NULL, "100ask_led%d", 0);





    free_gpios:
        for (i = i - 1; i >= 0; i--) {
            gpiod_put(gpio_keys[i].desc);
        }



    dev_info(dev, "Driver initialized successfully, major = %d\n", major);



    return 0;
}

/* 平台设备移除函数 */
static int my_gpio_remove(struct platform_device *pdev)
{
    int i;

    device_destroy(gpio_class, MKDEV(major, 0));
    class_destroy(gpio_class);
    unregister_chrdev(major, DEVICE_NAME);

    for (i = 0; i < gpio_count; i++) {
        gpiod_put(gpio_keys[i].desc);
        free_irq(gpio_keys[i].irq, &gpio_keys[i]);
        del_timer(&gpio_keys[i].key_timer);
        tasklet_kill(&gpio_keys[i].key_tasklet);
    }
    kfree(gpio_keys);

    printk(KERN_INFO "%s: Driver removed\n", DEVICE_NAME);
    return 0;
}







/* 设备树匹配表 */
static const struct of_device_id my_gpio_of_match[] = {
    { .compatible = "my_interrupts,my_drv" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, my_gpio_of_match);

/* 平台驱动定义 */
static struct platform_driver my_gpio_driver = {
    .probe = my_gpio_probe,
    .remove = my_gpio_remove,
    .driver = {
        .name = "my_interrupts",
        .of_match_table = my_gpio_of_match,
    },
};

/* 模块初始化和退出 */
static int __init my_module_init(void)
{
    printk(KERN_INFO "%s: Initializing module\n", DEVICE_NAME);
    return platform_driver_register(&my_gpio_driver);
}

static void __exit my_module_exit(void)
{
    platform_driver_unregister(&my_gpio_driver);
    printk(KERN_INFO "%s: Module exited\n", DEVICE_NAME);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("GPIO Interrupt Driver for Raspberry Pi");