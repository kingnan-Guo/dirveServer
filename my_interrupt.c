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
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/gpio/consumer.h>



#define _NAME "key_diver"

static int major; // 设备主号

static struct class *_class;
static struct gpio_desc *_gpio;



// 定义全局的 结构体数组 用于保存 gpio  flags irq

struct gpio_key {
    int gpio;
    enum of_gpio_flags flag;
    int irq;
};

// 我感觉需要定义一个数据 ，但老师 有点浪费 说定义一个结构体指针，之后再分配
static struct gpio_key *gpio_keys;


static int _open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t _read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    char level = '0';
    if(gpiod_get_value(_gpio)){
        level = '1';
    }
    if (copy_to_user(buffer, &level, 1)) {
        printk(KERN_ERR "Failed to copy GPIO status to user\n");
        return -EFAULT;
    }
    return 1;  // 返回读取的字节数
}

// 写入设备
static ssize_t _write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return len;  // 返回写入的数据字节数
}

static int _release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static struct file_operations _fops = {
    .owner = THIS_MODULE,
    .read = _read,
    .write = _write,
    .open = _open,
    .release = _release
};




// 中断处理函数
static irqreturn_t gpio_key_isr(int irq, void *dev_id){
    struct gpio_key *gpio_key = dev_id;
    // 读取对应的 引脚 来获取电平

    printk("key %d  value %d\n", irq, gpio_get_value(gpio_key->gpio));


    return IRQ_HANDLED;// 返回 IRQ_HANDLED 表示中断处理完成

}


/// @brief 
/// @param pdev 
/// @return 
static int _chip_gpio_probe(struct platform_device *pdev){
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);


    int count;// gpio 的数量
    struct device_node *node = pdev->dev.of_node;// 获取设备节点
    enum of_gpio_flags flag;// gpio 的 flags
    int gpio;// 引脚编号
    int irq;// 中断号
    int err;
    


    // 获取 gpio 的数量
    // pdev->dev.of_node 是 device_node 结构体，表示设备树节点
    count = of_gpio_count(node);

    /**
     * 申请内存空间，用于保存 gpio 的 flags 和 irq
     * kzalloc : k 表示内核， z 表示清零， alloc 表示分配内存, 分配好了之后 把内容清空
     * kzalloc 传参
     *  1、申请的内存大小
     *  2、内存分配的标志
     *  3、返回值是一个指针，指向分配的内存空间
     * 
     * GFP_KERNEL: 用于内核线程，可以睡眠
     * GFP_ATOMIC: 用于中断上下文，不可以睡眠
     * GFP_DMA: 用于DMA内存，可以睡眠
     * GFP_USER: 用于用户空间，可以睡眠
     * GFP_HIGHMEM: 用于高端内存，可以睡眠
     * GFP_ZERO: 分配内存后，将内存清零
     * GFP_NOIO: 不允许分配高端内存
     * GFP_NOFS: 不允许分配文件系统内存
     * GFP_NOWAIT: 不等待内存分配成功
     * GFP_ATOMIC: 不允许睡眠
     */
    gpio_keys = kzalloc(sizeof(struct gpio_key) * count, GFP_KERNEL);
    int i;
    // 要把设备 节点 里面 的 的每一个 gpio 引脚 都 取出来，取出来之后 再转换成 中断号
    for (i = 0; i < count; i++)
    {
        // gpiod_get_index(node, i, );


        // 获得 gpio 的 flags, 获取 i 个 gpio 的编号 ， 从 device_node 里面取出来,
        //  第 i 个 gpio 
        // 并且把 flags 保存下来
        gpio = of_get_gpio_flags(node, i, &flag);
        // 使用 gpio 转换成 中断号， 需要知道这个 引脚的 编号
        irq = gpio_to_irq(gpio);


        gpio_keys[i].gpio = gpio;
        gpio_keys[i].irq = irq;
        gpio_keys[i].flag = flag;

        
        // 申请中断，中断号，中断处理函数，中断标志
        // 参数
        // 1、中断号； 申请 哪一个中断，
        // 2、中断处理函数； gpio_key_isr; 当发生中断的时候内核 会 调用 gpio_key_isr 函数， 并且会把 &gpio_keys[i]  参数 传给 gpio_key_isr
        // 3、中断标志; flags 用来描述中断的类型，是上升沿触发，还是下降沿触发，还是高电平触发，还是低电平触发; IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING  双边沿 触发

        // 4、中断处理函数的名称
        // 5、中断处理函数 的 指针

        err = request_irq(irq, gpio_key_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "gpio_key", &gpio_keys[i]);
    }
    


    return 0;


}

/// @brief 清除驱动
/// @param pdev 
/// @return 
static int _chip_gpio_remove(struct platform_device *pdev){


    int count;// gpio 的数量
    struct device_node *node = pdev->dev.of_node;// 获取设备节点

    count = of_gpio_count(node);
    int i;
    for (i = 0; i < count; i++)
    {
        free_irq(gpio_keys[i].irq, &gpio_keys[i]);// 释放中断
    }
    




    device_destroy(_class, MKDEV(major, 0)); // 销毁设备
    class_destroy(_class);// 销毁 class
    unregister_chrdev(major, _NAME);// 卸载驱动 注销字符设备
    // 清除 gpio
    gpiod_put(_gpio);
    return 0;
}



static const struct of_device_id _chip_gpio_of_match[] = {
    { .compatible = "my_interrupts,my_drv" }// my_board_device,my_drv 这个值 在 dtb 上配好的 
};

static struct platform_driver _chip_gpio_dirver = {
    .probe = _chip_gpio_probe,
    .remove = _chip_gpio_remove,
    .driver = {
        .name = "my_interrupts",// 名字 用来跟 platform_device 配对 如果配对成功
        .of_match_table = _chip_gpio_of_match
    },
};







// 模块加载时执行的函数 
static int __init device_init(void) {
    printk(KERN_INFO "========= %s %s %d  ========= \n", __FILE__, __FUNCTION__, __LINE__);
    // 入口函数里 注册 _chip_gpio_dirver 结构体
    int err;
    err = platform_driver_register(&_chip_gpio_dirver);// 注册 _chip_gpio_dirver 会调用  probe
    return err;
}

// 模块卸载时执行的函数
static void __exit device_exit(void) {
    platform_driver_unregister(&_chip_gpio_dirver);// 会调用 remove
    printk(KERN_INFO "Entering _exit\n");
}

// 定义模块的加载和卸载函数
module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");


