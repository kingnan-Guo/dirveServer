#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>      

#define DEVICE_NAME "my_test_device"
static int major; // 设备主号

static ssize_t my_test_device_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return len;  // 返回写入的数据字节数
}

static ssize_t my_test_device_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;  // 返回读取的字节数
}

static int my_test_device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static int my_test_device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static struct file_operations my_test_device_fops = {
    .owner = THIS_MODULE,
    .read = my_test_device_read,
    .write = my_test_device_write,
    .open = my_test_device_open,
    .release = my_test_device_release
};

// 模块加载时执行的函数
static int __init my_test_device_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &my_test_device_fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register character device.\n");
        return major;
    }
    printk(KERN_INFO "my_test_device registered with major number %d.\n", major);
    return 0;
}

// 模块卸载时执行的函数
static void __exit my_test_device_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "my_test_device unregistered.\n");
}

// 定义模块的加载和卸载函数
module_init(my_test_device_init);
module_exit(my_test_device_exit);

MODULE_LICENSE("GPL");