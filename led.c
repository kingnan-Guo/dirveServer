#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>


// GPIO寄存器基地址（树莓派3B+）
// #define BCM2711_GPIO_BASE   0x3F200000;// GPIO功能选择寄存器
// #define GPFSEL0             (GPIO_BASE + 0x00)
// #define GPFSEL1             (GPIO_BASE + 0x04)
// #define GPFSEL2             (GPIO_BASE + 0x08)
// #define GPFSEL3             (GPIO_BASE + 0x0C)

// static volatile unsigned int *RCC


// static volatile unsigned int *gpio_base = (volatile unsigned int *)BCM2711_GPIO_BASE;
// static volatile unsigned int *gpio_base = (volatile unsigned int *)ioremap(BCM2711_GPIO_BASE, 0x1000);



#define BCM2837_GPIO_BASE   0x3F200000  // GPIO寄存器基地址
#define GPIO_REG_SIZE       (4 * 1024)  // 定义了 GPIO 寄存器块的大小，(4 * 1024) 是 4 KB，这个大小可以用来映射 GPIO 寄存器的内存区域
#define GPFSEL1             0x04        // 定义了 GPIO 功能选择寄存器 1 的偏移地址，0x04。这个寄存器用于设置 GPIO 引脚的工作模式，比如输入、输出或备用功能。每个 GPFSEL 寄存器控制多个 GPIO 引脚的功能
#define GPSET0              0x1C        // 定义了 GPIO 设置寄存器 0 的偏移地址，0x1C。这个寄存器用于设置 GPIO 引脚为高电平（输出高）。如果需要将某个 GPIO 引脚置为高电平，就需要向这个寄存器写入对应引脚的位
#define GPCLR0              0x28        // 定义了 GPIO 清除寄存器 0 的偏移地址，0x28。这个寄存器用于将 GPIO 引脚设置为低电平（输出低）。如果需要将某个 GPIO 引脚清零，就需要向这个寄存器写入对应引脚的位




static int major; // 主设备号
#define DEVICE_NAME "my_led";

static struct class *my_led_class;






static int led_open(struct inode *node, struct file *file){
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

    /**
     * 1. enable gpio  使能 gpio 
     * 2. configure pin as gpio 配置引脚为 gpio
     * 3. configure gpio as  output 配置 gpio 为输出 引脚
     * 
     */
    
    
    return 0;
}


static ssize_t led_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {

}

static ssize_t led_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    // copy_from_user : get data  frome app

    // to set gpio  register : out 1 / 0





    return len;
}

static int led_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}


static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .release = led_release
}


static int __init led_init(void){
    // 注册 字符设备驱动
    major = register_chrdev(0, DEVICE_NAME, &led_fops);

    // iormap 映射寄存器


    // 创建设备类
    // 为 THIS_MODULE  模块创建一个类
    my_led_class = class_create(THIS_MODULE, "my_led_class");

    // 创建设备节点  /dev/led
    device_create(led_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);


    return 0;
}


static void __exit led_exit(void){
    // 删除设备节点
    device_destroy(my_led_class, MKDEV(major, 0));
    // 销毁类
    class_destroy(my_led_class);
    // 释放设备号
    unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "my_led unregistered.\n");
}

module_init(led_init);
module_exit(led_exits);
MODULE_LICENSE("GPL");
