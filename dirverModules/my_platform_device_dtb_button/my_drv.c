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



#include "my_op.h"
#include "my_drv.h"

#define DEVICE_NAME "my_board_button"

// #define DEVICE_NUM 2

static int major; // 设备主号

static struct class *device_class;

struct my_operations *p_my_device_operations;


static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

    // 根据 次设备号 初始化 led
    /**
     * MINOR(inode->i_rdev)  获取次设备号
     */

    // int minor = MINOR(inode->i_rdev);// 获取次设备号
    int minor = iminor(inode);// 获取次设备号
    p_my_device_operations->init(minor);// 初始化 LED

    return 0;
}

static ssize_t device_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    
    struct inode  *inode = file_inode(file);// 获取文件的 inode
    int minor = iminor(inode);// 获取次设备号
    char level;

    // int ret = p_my_device_operations->read(minor, buffer);
    level = p_my_device_operations->read(minor, buffer);
    // err = copy_to_user(buffer, &level, 1);
    if (copy_to_user(buffer, &level, 1)) {
        printk(KERN_ERR "Failed to copy GPIO status to user\n");
        return -EFAULT;
    }



    return 1;  // 返回读取的字节数
}

// 写入设备
// write(fd,  &val, sizeof(val));
static ssize_t device_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

    // if (len > sizeof(my_buffer)) {
    //     len = sizeof(my_buffer);  // 限制写入长度
    // }

    char status_char;


    // 从用户空间读取数据到内核空间
    // 检查返回值
    if (copy_from_user(&status_char, buffer, len)) {
        printk(KERN_WARNING "Failed to copy data from user space.\n");
        return -EFAULT;  // 返回错误码
    }



    int status = status_char - '0';
    printk(KERN_INFO "status  == %d \n", status);
    // 根据次设备号 和 status 控制 LED
    struct inode  *inode = file_inode(file);// 获取文件的 inode
    int minor = iminor(inode);// 获取次设备号

    p_my_device_operations->ctl(minor, status); // 

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
    printk(KERN_INFO "========= %s %s %d  ========= \n", __FILE__, __FUNCTION__, __LINE__);
    // char class_name[30];
    // char device_name_buf[30];


    major = register_chrdev(0, DEVICE_NAME, &device_fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register character device.\n");
        return major;
    }

    // 创建设备类
    // 为 THIS_MODULE  模块创建一个类
    // snprintf(class_name, sizeof(class_name), "%s_class", DEVICE_NAME);
    device_class = class_create("my_board_button_class");
    if(IS_ERR(device_class)){
        // pr_err("failed to allocate class\n");
        printk("failed to allocate class\n");
        return PTR_ERR(device_class);
    }

    // 在入口函数中 获得 get_board_operations； 然后 使用这个指针来操作 单板相关的代码
    // p_my_device_operations  = get_board_operations();

    // device_create(device_class, NEV(major, 0), NULL, DEVICE_NAME + "_0");

    // for(int i = 0; i < p_my_device_operations->num; i++){
    //     // snprintf(device_name_buf, sizeof(device_name_buf), "%s_%d", DEVICE_NAME, i);
    //     // 创建设备节点
    //     // device_create(device_class, NULL, MKDEV(major, i), NULL, device_name_buf);
    //     _device_create(i);
    // }






    printk(KERN_INFO "device registered with major number %d.\n", major);

    return 0;
}



// 模块卸载时执行的函数
static void __exit device_exit(void) {
    printk(KERN_INFO "Entering device_exit\n");

    // 先删除设备节点
    if (device_class) {


        // 删除设备类
        class_destroy(device_class);
        device_class = NULL;
    }

    // // 调用板级 `exit` 方法
    // if (p_my_device_operations && p_my_device_operations->exit) {
    //     printk(KERN_INFO "Calling board_exit\n");
    //     p_my_device_operations->exit(0);
    // }

    printk(KERN_INFO "p_my_device_operations = %p\n", p_my_device_operations);

    if (p_my_device_operations) {
        printk(KERN_INFO "Calling board_exit\n");
        p_my_device_operations->exit(0);

    } else {
        printk(KERN_WARNING "p_my_device_operations is invalid or module not loaded\n");
    }


    // 注销字符设备
    unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "Device unregistered\n");
}








// 封装 device_create 
void _device_create(int minor){
    char device_name_buf[30];
    snprintf(device_name_buf, sizeof(device_name_buf), "%s_%d", DEVICE_NAME, minor);
    device_create(device_class, NULL, MKDEV(major, minor), NULL, device_name_buf);//创建 文件系统 的设备节点; 应用程序 通过文件系统的设备 节点 访问 硬件  
}
// 别的函数要使用此 函数 必须 导出来
// 所以 别的函数要使用  _device_create  必须 先 加载 my_drv.c
EXPORT_SYMBOL(_device_create);



void _device_destroy(int minor){
    // device_destroy(device_class, MKDEV(major, minor));
    device_destroy(device_class, MKDEV(major, minor));
}
EXPORT_SYMBOL(_device_destroy);


// 底层向上册 注册函数
void _register_device_operations(struct my_operations *opr){
    // p_my_device_operations 由 底层 主动提供
    p_my_device_operations = opr;
}
EXPORT_SYMBOL(_register_device_operations);




// 定义模块的加载和卸载函数
module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");