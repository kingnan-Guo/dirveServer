#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "my_test_device"
static int major; // 设备主号
static unsigned char my_buffer[1024]; // 内核空间缓冲区

static struct class *my_test_device_class;


static int my_test_device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t my_test_device_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    // 读取数据到用户空间
    // int copy_to_user(void __user *dst, const void *src, unsigned size)
    // unsigned long len = len > 100 ? 100 : len;
    // copy_to_user(buffer, my_buffer, len);

    // 检查返回值
    if (copy_to_user(buffer, my_buffer, len)) {
        printk(KERN_WARNING "Failed to copy data to user space.\n");
        return -EFAULT;  // 返回错误码
    }


    return len;  // 返回读取的字节数
}

static ssize_t my_test_device_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

    if (len > sizeof(my_buffer)) {
        len = sizeof(my_buffer);  // 限制写入长度
    }

    // 从用户空间读取数据到内核空间
    // copy_from_user(my_buffer, buffer, len);
    // 检查返回值
    if (copy_from_user(my_buffer, buffer, len)) {
        printk(KERN_WARNING "Failed to copy data from user space.\n");
        return -EFAULT;  // 返回错误码
    }


    return len;  // 返回写入的数据字节数
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

    // 创建设备类
    // 为 THIS_MODULE  模块创建一个类
    my_test_device_class = class_create(THIS_MODULE, "my_test_device_class");
    if(IS_ERR(my_test_device_class)){
        // pr_err("failed to allocate class\n");
        printk("failed to allocate class\n");
        return PTR_ERR(my_test_device_class);
    }
    /**
     * 创建设备节点
     *  params:
     *      1. class: 设备类
     *      2. dev_t: 设备号
     *      3. const char *: 设备节点名称
     *      4. dev_t: 设备号            MKDEV(major, 0) 是个整数
     *      5. const char *: 设备节点名称
     *    *      6. void *: 私有数据
     * 
     *  会去 创建一个 设备节点 /dev/my_test_device
     */
    device_create(my_test_device_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);


    printk(KERN_INFO "my_test_device registered with major number %d.\n", major);
    return 0;
}

// 模块卸载时执行的函数
static void __exit my_test_device_exit(void) {

    // 打印 my_buffer
    printk(KERN_INFO "my_buffer: %s\n", my_buffer);

    // 销毁类下面的 设备节点
    device_destroy(my_test_device_class, MKDEV(major, 0));
    // 销毁类
    class_destroy(my_test_device_class);

    
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "my_test_device unregistered.\n");
}

// 定义模块的加载和卸载函数
module_init(my_test_device_init);
module_exit(my_test_device_exit);

MODULE_LICENSE("GPL");