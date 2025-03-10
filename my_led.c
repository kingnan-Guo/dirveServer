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

static volatile unsigned int *GPIO_BASE = NULL;

#define GPFSEL0 ((volatile unsigned int *)(GPIO_BASE + (GPFSEL0_OFFSET / 4)))
#define GPFSEL1 ((volatile unsigned int *)(GPIO_BASE + (GPFSEL1_OFFSET / 4)))
#define GPFSEL2 ((volatile unsigned int *)(GPIO_BASE + (GPFSEL2_OFFSET / 4)))
#define GPSET0  ((volatile unsigned int *)(GPIO_BASE + (GPSET0_OFFSET / 4)))
#define GPCLR0  ((volatile unsigned int *)(GPIO_BASE + (GPCLR0_OFFSET / 4)))

static int GPIO_PIN = 27;
static int major;
#define DEVICE_NAME "my_led"

static struct class *my_led_class;

static int led_open(struct inode *node, struct file *file) {
    unsigned int gpfsel2_val;
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    gpfsel2_val = *GPFSEL2;
    printk(KERN_INFO "GPFSEL2 before: 0x%x\n", gpfsel2_val);
    gpfsel2_val &= ~(0x7 << 21);
    gpfsel2_val |= (0x1 << 21);
    *GPFSEL2 = gpfsel2_val;
    printk(KERN_INFO "GPFSEL2 after: 0x%x\n", *GPFSEL2);
    return 0;
}

static ssize_t led_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
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
