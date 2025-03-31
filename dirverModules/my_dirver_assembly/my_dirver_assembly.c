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
#include <asm/pgtable.h>
#include <linux/mm.h>
#include <linux/slab.h>

#include "my_asm_add.h"

static int major;
#define DEVICE_NAME "my_dirver_assembly"

static struct class *my_dirver_assembly_class;


static int my_open(struct inode *node, struct file *file) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return  len;
}

static ssize_t my_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return  len;
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
    .release = my_release,

};

static int __init my_dirver_assembly_init(void) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);


    // 使用 汇编
    int result = my_asm_add(3, 5);
    printk(KERN_INFO "Result of my_asm_add: %d\n", result);


    major = register_chrdev(0, DEVICE_NAME, &my_fops);
    if (major < 0) {
        printk(KERN_ERR "Failed to register chrdev\n");
        return major;
    }

    my_dirver_assembly_class = class_create("my_dirver_assembly_class");
    if (IS_ERR(my_dirver_assembly_class)) {
        printk(KERN_ERR "Failed to create class\n");
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(my_dirver_assembly_class);
    }

    device_create(my_dirver_assembly_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    printk(KERN_INFO "my_dirver_assembly initialized\n");
    return 0;
}

static void __exit my_dirver_assembly_exit(void) {
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    
    device_destroy(my_dirver_assembly_class, MKDEV(major, 0));
    class_destroy(my_dirver_assembly_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "my_dirver_assembly unregistered.\n");

}

module_init(my_dirver_assembly_init);
module_exit(my_dirver_assembly_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kingnan Guo");
MODULE_DESCRIPTION("my_dirver_assembly driver for Raspberry Pi");
