#include "linux/i2c.h"
#include <linux/module.h>
#include <linux/poll.h>
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
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/timer.h>
#include "asm/uaccess.h"
#include "linux/delay.h"

#define _NAME "my_i2c_diver"

struct i2c_client *global_client;

static int major; // 设备主号
static struct class *_class;

DECLARE_WAIT_QUEUE_HEAD(my_wait_queue);

static int _open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t _read(struct file *file, char __user *buffer, size_t size, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    int err;
    unsigned char *kern_buf; // 内核空间缓冲区
    struct i2c_msg msgs[2];  // i2c 消息结构体数组, 初始化 i2c_msg

    /** 从 offset 读取 size 字节 */
    kern_buf = kmalloc(size + 1, GFP_KERNEL); // 多分配1字节用于地址
    if (!kern_buf) {
        return -ENOMEM;
    }

    /**
     * 读取数据 
     * 读操作 
     * 1 发一次写操作， 把地址发给 AT24C02
     * 2 发一次读操作， 得到数据
     */
    msgs[0].addr = global_client->addr;
    msgs[0].flags = 0; // 0 是 写操作
    kern_buf[0] = (unsigned char)(*offset); // 从 offset 开始读取
    msgs[0].buf = kern_buf;
    msgs[0].len = 1;

    msgs[1].addr = global_client->addr;
    msgs[1].flags = I2C_M_RD; // I2C_M_RD 是 1 读操作
    msgs[1].buf = kern_buf + 1; // 数据存储在 kern_buf[1] 开始
    msgs[1].len = size;

    err = i2c_transfer(global_client->adapter, msgs, 2);
    if (err < 0) {
        kfree(kern_buf);
        return err;
    }

    /** copy to user */
    err = copy_to_user(buffer, kern_buf + 1, size);
    if (err) {
        kfree(kern_buf);
        return -EFAULT;
    }

    *offset += size; // 更新偏移量
    kfree(kern_buf);
    return size; // 返回读取的字节数
}

// 写入设备
static ssize_t _write(struct file *file, const char __user *buffer, size_t size, loff_t *offset) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    int err;
    unsigned char kern_buf[9]; // 内核空间缓冲区, 用于存储要写入的数据，为什么是 9 呢？因为 i2c 一次最多可以写入 8 个字节，再加上地址，所以是 9 个字节
    int len;
    size_t total_written = 0;
    unsigned char addr = (unsigned char)(*offset); // 从 offset 开始写入
    struct i2c_msg msg; // 单次写操作只用一个消息

    /** copy from user */
    while (size > 0) {
        if (size > 8) {
            len = 8;
        } else {
            len = size;
        }

        /**
         * 写操作
         * 循环写入数据，每次写入最多 9 个字节，包括地址和数据
         */
        kern_buf[0] = addr; // 设置写入地址
        err = copy_from_user(kern_buf + 1, buffer + total_written, len);
        if (err) {
            return -EFAULT;
        }

        msg.addr = global_client->addr;
        msg.flags = 0; // 0 是 写操作
        msg.buf = kern_buf;
        msg.len = len + 1; // 地址 + 数据

        err = i2c_transfer(global_client->adapter, &msg, 1);
        if (err < 0) {
            return err;
        }

        size -= len;
        total_written += len;
        addr += len; // 更新地址
        mdelay(20); // AT24C02 写周期需要延时
    }

    *offset += total_written; // 更新偏移量
    return total_written; // 返回写入的字节数
}

static int _release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

// _poll
static unsigned int _poll(struct file *file, struct poll_table_struct *wait) {
    return 0;
}

// _fasync
static int _fasync(int fd, struct file *file, int mode) {
    return 0;
}

static struct file_operations _fops = {
    .owner = THIS_MODULE,
    .read = _read,
    .write = _write,
    .open = _open,
    .release = _release,
    .poll = _poll, // poll 机制
    .fasync = _fasync, // 异步通知
};

static int my_i2c_driver_probe(struct i2c_client *client) {
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    struct i2c_adapter *i2c_adapter = client->adapter;

    if (!i2c_check_functionality(i2c_adapter, I2C_FUNC_I2C)) {
        printk(KERN_ERR "I2C functionality not supported\n");
        return -ENODEV;
    }

    /** 记录 client */
    global_client = client;

    /** 注册字符设备 */
    char device_name_buf[30];
    major = register_chrdev(0, _NAME, &_fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register character device.\n");
        return major;
    }

    snprintf(device_name_buf, sizeof(device_name_buf), "%s_%s", _NAME, "_class");
    _class = class_create(device_name_buf);
    if (IS_ERR(_class)) {
        printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        unregister_chrdev(major, _NAME);
        printk("failed to allocate class\n");
        return PTR_ERR(_class);
    }

    int minor = 0;
    snprintf(device_name_buf, sizeof(device_name_buf), "%s_%d", _NAME, minor);
    device_create(_class, NULL, MKDEV(major, minor), NULL, device_name_buf); // 创建 文件系统 的设备节点; 应用程序 通过文件系统的设备 节点 访问 硬件
    return 0;
}

static void my_i2c_driver_remove(struct i2c_client *client) {
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    /** 反 注册字符设备 */
    device_destroy(_class, MKDEV(major, 0));
    class_destroy(_class);
    unregister_chrdev(major, _NAME);
}

static const struct of_device_id of_match[] = {
    { .compatible = "kingnan,i2c_dev", .data = NULL },
    { /* END OF LIST */ },
};

static const struct i2c_device_id my_i2c_driver_ids[] = {
    { "kingnan,i2c_dev", (kernel_ulong_t)NULL },
    { /* sentinel */ }
};

static struct i2c_driver my_i2c_driver = {
    .driver = {
        .name = "my_i2c_driver",
        .of_match_table = of_match_ptr(of_match),
    },
    .probe = my_i2c_driver_probe,
    .remove = my_i2c_driver_remove,
    .id_table = my_i2c_driver_ids,
};

/* 1. 入口函数 */
static int __init my_i2c_dirver_init(void) {
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    /* 1.1 注册一个my_i2c_driver */
    return i2c_add_driver(&my_i2c_driver);
};

/* 2. 出口函数 */
static void __exit my_i2c_dirver_exit(void) {
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    /* 2.1 反注册my_i2c_driver */
    i2c_del_driver(&my_i2c_driver);
};

module_init(my_i2c_dirver_init);
module_exit(my_i2c_dirver_exit);

MODULE_AUTHOR("Your Name");
MODULE_LICENSE("GPL");