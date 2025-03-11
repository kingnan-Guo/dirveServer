#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>


#define DEVICE_NAME "my_device"

#define DEVICE_NUM 2

static int major; // 设备主号
static unsigned char my_buffer[1024]; // 内核空间缓冲区
static struct class *device_class;
// static char *device_name = "my_device";


static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    // 检查返回值
    if (copy_to_user(buffer, my_buffer, len)) {
        printk(KERN_WARNING "Failed to copy data to user space.\n");
        return -EFAULT;  // 返回错误码
    }
    return len;  // 返回读取的字节数
}

static ssize_t device_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

    if (len > sizeof(my_buffer)) {
        len = sizeof(my_buffer);  // 限制写入长度
    }
    // 从用户空间读取数据到内核空间
    // 检查返回值
    if (copy_from_user(my_buffer, buffer, len)) {
        printk(KERN_WARNING "Failed to copy data from user space.\n");
        return -EFAULT;  // 返回错误码
    }

    return len;  // 返回写入的数据字节数
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static struct file_operations device_fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

// 模块加载时执行的函数
static int __init device_init(void) {
    
    char class_name[30];
    char device_name_buf[30];

    // 生成 class 名称
    



    major = register_chrdev(0, DEVICE_NAME, &device_fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register character device.\n");
        return major;
    }

    // 创建设备类
    // 为 THIS_MODULE  模块创建一个类
    snprintf(class_name, sizeof(class_name), "%s_class", DEVICE_NAME);
    device_class = class_create(class_name);
    if(IS_ERR(device_class)){
        // pr_err("failed to allocate class\n");
        printk("failed to allocate class\n");
        return PTR_ERR(device_class);
    }

    // device_create(device_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME + "_0");

    for(int i = 0; i < DEVICE_NUM; i++){

        snprintf(device_name_buf, sizeof(device_name_buf), "%s_%d", DEVICE_NAME, i);
        // 创建设备节点
        device_create(device_class, NULL, MKDEV(major, i), NULL, device_name_buf);
    }




    printk(KERN_INFO "device registered with major number %d.\n", major);

    return 0;
}

// 模块卸载时执行的函数
static void __exit device_exit(void) {
    // 打印 my_buffer
    printk(KERN_INFO "my_buffer: %s\n", my_buffer);
    // 销毁类下面的 设备节点
    // device_destroy(device_class, MKDEV(major, 0));
    for(int i = 0; i < DEVICE_NUM; i++){
        device_destroy(device_class, MKDEV(major, i));
    }
    // 销毁类
    class_destroy(device_class);
    
    unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "device unregistered.\n");
}

// 定义模块的加载和卸载函数
module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");