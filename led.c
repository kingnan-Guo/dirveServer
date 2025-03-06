#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

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
    unregister_chrdev(major, "led");

    printk(KERN_INFO "my_led unregistered.\n");
}

module_init(led_init);
module_exit(led_exits);
MODULE_LICENSE("GPL");
