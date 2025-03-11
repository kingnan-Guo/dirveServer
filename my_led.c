#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/io.h>

#define BCM2837_PERI_BASE   0x3F000000
#define BCM2837_GPIO_BASE   (BCM2837_PERI_BASE + 0x200000)

#define GPFSEL0_OFFSET      0x00
#define GPFSEL1_OFFSET      0x04
#define GPFSEL2_OFFSET      0x08
#define GPSET0_OFFSET       0x1C
#define GPCLR0_OFFSET       0x28
#define GPLEV0_OFFSET       0x34

static volatile unsigned int *GPIO_BASE = NULL;

#define GPFSEL0 ((volatile unsigned int *)(GPIO_BASE + (GPFSEL0_OFFSET / 4)))
#define GPFSEL1 ((volatile unsigned int *)(GPIO_BASE + (GPFSEL1_OFFSET / 4)))
#define GPFSEL2 ((volatile unsigned int *)(GPIO_BASE + (GPFSEL2_OFFSET / 4)))
#define GPSET0  ((volatile unsigned int *)(GPIO_BASE + (GPSET0_OFFSET / 4)))
#define GPCLR0  ((volatile unsigned int *)(GPIO_BASE + (GPCLR0_OFFSET / 4)))
#define GPLEV0  ((volatile unsigned int *)(GPIO_BASE + (GPLEV0_OFFSET / 4))) // 读取 gpio 的状态
static int GPIO_PIN = 27;
static int major;
#define DEVICE_NAME "my_led"

static struct class *my_led_class;

static int led_open(struct inode *node, struct file *file) {

    /**
     * 1. enable gpio  使能 gpio ； 但是 rpi 是不需要 启动使能 gpio 的， 也不需要 配置 gpio 的时钟 ，stm32pm157 是需要的
     * 2. configure pin as gpio 配置引脚为 gpio
     * 3. configure gpio as  output 配置 gpio 为输出 引脚
     * 
     */


    unsigned int gpfsel2_val;
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    gpfsel2_val = *GPFSEL2;
    printk(KERN_INFO "GPFSEL2 before: 0x%x\n", gpfsel2_val);
    // 清除 21-23 位 ；假设 GPFSEL2 是  0000 0000 0000 0000 0000 0000 0000 0000 ，那么 0x7  是 0000 0000 0000 0000 0000 0000 0000 0111 ，所以 0x7 << 21 是 0000 0000 1110 0000 0000 0000 0000 0000
    gpfsel2_val &= ~(0x7 << 21);
     // 设置 21-23 位 ；假设 GPFSEL2 是  0000 0000 0000 0000 0000 0000 0000 0000 ，那么 0x1  是 0000 0000 0000 0000 0000 0000 0000 0001 ，所以 0x1 << 21 是 0000 0000 0010 0000 0000 0000 0000 0000
    gpfsel2_val |= (0x1 << 21);
    *GPFSEL2 = gpfsel2_val;
    printk(KERN_INFO "GPFSEL2 after: 0x%x\n", *GPFSEL2);
    return 0;
}

static ssize_t led_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    // 读取 gpio 的状态
    char  gpio_value;
    unsigned int gpio_state;

    gpio_state = *GPLEV0; // GPLEV0 寄存器包含所有的 GPIO 引脚的 当前状态
    // 读取 27 位的状态
    if(gpio_state & (1 << GPIO_PIN)){
        gpio_value = '1';
    } else {
        gpio_value = '0';
    }
    if(len < 1){
        return -EFAULT;
    }

    if(copy_to_user(buffer, &gpio_value, 1)){
        return -EFAULT;
    }
    
    printk(KERN_INFO "GPIO %d  static %c \n", GPIO_PIN, gpio_value);

    return 1;
}

static ssize_t led_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    char value;
    if (len < 1) return -EINVAL;
    if (copy_from_user(&value, buffer, 1)){
        return -EFAULT;
    }

    // printk(KERN_INFO "%s %s %d Value: %c\n", __FILE__, __FUNCTION__, __LINE__, value);
    if (value == '1') {
        printk(KERN_INFO "Turning LED ON\n");
        *GPSET0 = (1 << GPIO_PIN);
        printk(KERN_INFO "GPSET0 after: 0x%x\n", *GPSET0);
    } else if (value == '0') {
        printk(KERN_INFO "Turning LED OFF\n");
        *GPCLR0 = (1 << GPIO_PIN);
        printk(KERN_INFO "GPCLR0 after: 0x%x\n", *GPCLR0);
    } else {
        return -EINVAL;
    }
    return len;
}

static int led_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .release = led_release
};

static int __init led_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &led_fops);
    if (major < 0) {
        printk(KERN_ERR "Failed to register chrdev\n");
        return major;
    }

    printk(KERN_INFO "Mapping BCM2837_GPIO_BASE: 0x%x\n", BCM2837_GPIO_BASE);
    GPIO_BASE = ioremap(BCM2837_GPIO_BASE, 0x1000);
    if (!GPIO_BASE) {
        printk(KERN_ERR "Failed to map GPIO registers\n");
        unregister_chrdev(major, DEVICE_NAME);
        return -ENOMEM;
    }
    printk(KERN_INFO "GPIO_BASE mapped to: %p\n", GPIO_BASE);
    printk(KERN_INFO "GPFSEL2 address: %p, value: 0x%x\n", GPFSEL2, *GPFSEL2);

    my_led_class = class_create("my_led_class");
    if (IS_ERR(my_led_class)) {
        printk(KERN_ERR "Failed to create class\n");
        iounmap(GPIO_BASE);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(my_led_class);
    }

    device_create(my_led_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    printk(KERN_INFO "my_led initialized\n");
    return 0;
}

static void __exit led_exit(void) {
    device_destroy(my_led_class, MKDEV(major, 0));
    class_destroy(my_led_class);
    iounmap(GPIO_BASE);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "my_led unregistered.\n");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple LED driver for Raspberry Pi GPIO27");
