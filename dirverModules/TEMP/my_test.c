#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/io.h>


static int major;
#define DEVICE_NAME "my_test"

static struct class *my_test_class;

static int my_open(struct inode *node, struct file *file) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 1;
}

static ssize_t my_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return len;
}

static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_read,
    .write = my_write,
    .release = my_release
};

static int __init test_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &my_fops);
    if (major < 0) {
        printk(KERN_ERR "Failed to register chrdev\n");
        return major;
    }

    my_test_class = class_create("my_test_class");
    if (IS_ERR(my_test_class)) {
        printk(KERN_ERR "Failed to create class\n");
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(my_test_class);
    }

    device_create(my_test_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    printk(KERN_INFO "my_test initialized\n");
    return 0;
}

static void __exit test_exit(void) {
    device_destroy(my_test_class, MKDEV(major, 0));
    class_destroy(my_test_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "my_test unregistered.\n");
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("test driver for Raspberry Pi");
