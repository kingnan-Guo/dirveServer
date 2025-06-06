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
    unsigned char *kern_buf;// 内核空间缓冲区


    struct i2c_msg msgs[2];// i2c 消息结构体数组, 初始化 i2c_msg


    
    /** 从 地址 0 读取 size 字节 */

    kern_buf = kmalloc(size, GFP_KERNEL);// kmalloc 是 申请  ；GFP_KERNEL 是 分配内核空间内存的标志

    /**
     * 
     * 读取数据 
     * 读操作 
     * 1 发一次写操作， 把 地址 0 发给 AT24C02, 表示要从 0 地址读数据
     * 2 发一次读操作， 得到数据
     */

    msgs[0].addr = global_client->addr; // 设置 i2c 设备地址
    msgs[0].flags = 0;// 0 是 写操作
    kern_buf[0] = 0; // 要写入的数据
    msgs[0].buf = kern_buf;// 要写入的数据
    msgs[0].len = 1;// 要写入的数据长度

    msgs[1].addr = global_client->addr; // 设置 i2c 设备地址
    msgs[1].flags = I2C_M_RD;// I2C_M_RD 是 1 读操作
    msgs[1].buf = kern_buf;// 要读取的数据
    msgs[1].len = size;// 要读取的数据长度


    // i2c 传输
    err = i2c_transfer(global_client->adapter, msgs, 2);// 


    /** copy to user */
    err = copy_to_user(buffer, kern_buf, size);

    kfree(kern_buf); // 释放内核空间缓冲区

    return 0;  // 返回读取的字节数
}

// 写入设备
// write(fd,  &val, sizeof(val));
static ssize_t _write(struct file *file, const char __user *buffer, size_t size, loff_t *offset) {
    
    printk(KERN_INFO "%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
    int err;
    unsigned char kern_buf[9];// 内核空间缓冲区, 用于存储要写入的数据，为什么是 9 呢？因为 i2c 一次最多可以写入 8 个字节，再加上地址，所以是 9 个字节
	int len;
	unsigned char addr = 0;
    struct i2c_msg msgs[1];// i2c 消息结构体数组, 初始化 i2c_msg


    /** copy from user */

    while (size > 0) {
        if(size > 8){
            len = 8;
        } else {
            len = size;
        }
        
        size -= len;

        err = copy_from_user(kern_buf + 1, buffer, len);
        buffer += len;// 这个是  buffer 的偏移量，每次写入 8 个字节，所以每次都要加 8？？？？？

        /**
         * 写操作
         * 
         * 循环写入数据 ，每次写入 9 个字节，包括地址和数据
         * 
         * 
         */
        msgs[0].addr = global_client->addr; // 设置 i2c 设备地址
        msgs[0].flags = 0;// 0 是 写操作
        msgs[0].buf = kern_buf;// 要写入的数据
        kern_buf[0] = addr; // 要写入的数据
        msgs[0].len = len+1;// 要写入的数据长度

        addr += len; // 更新地址



        // i2c 传输
        err = i2c_transfer(global_client->adapter, msgs, 1);// 

        mdelay(20);

    }
    



    return size;  // 返回写入的数据字节数
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
    .poll = _poll,// poll 机制
    .fasync = _fasync,// 异步通知
};







/*

client 表示 可以找到的 i2c_client 设备
*/
static int my_i2c_driver_probe(struct i2c_client *client) {
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    struct i2c_board_info *info = client->dev.platform_data; // 获取 i2c_client 的平台数据
    struct device_node *node = client->dev.of_node; // 获取 i2c_client 的设备树节点
    struct i2c_adapter *i2c_adapter = client->adapter;// i2c_adapter 是 i2c总线适配器
    // if (!i2c_check_functionality(i2c_adapter, I2C_FUNC_I2C)) {
    //     printk(KERN_ERR "I2C functionality not supported\n");
    //     return -ENODEV;
    // }

    /** 记录 clinet */


    global_client = client;


    /** 注册字符设备 */

    char device_name_buf[30];
    // 2、 注册  file_operations _fops
    major = register_chrdev(0, _NAME, &_fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register character device.\n");
        return major;
    }


    snprintf(device_name_buf, sizeof(device_name_buf), "%s_%s", _NAME, "_class");
    _class = class_create(device_name_buf);
    if(IS_ERR(_class)){
        printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

		unregister_chrdev(major, _NAME);

        printk("failed to allocate class\n");
        return PTR_ERR(_class);
    }




    int minor = 0;
    snprintf(device_name_buf, sizeof(device_name_buf), "%s_%d", _NAME, minor);
    device_create(_class, NULL, MKDEV(major, minor), NULL, device_name_buf);//创建 文件系统 的设备节点; 应用程序 通过文件系统的设备 节点 访问 硬件  





    
    return 0;
}

static void my_i2c_driver_remove(struct i2c_client *client){
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    /** 反 注册字符设备 */

    device_destroy(_class, MKDEV(major, 0));
    class_destroy(_class);
    unregister_chrdev(major, _NAME);

}



static const struct of_device_id of_match[] = {
	{ 
        .compatible = "kingnan,i2c_dev",		
        .data = NULL 
    },
	{ /* END OF LIST */ },
};


// 当前 是 站位  没有实际用途
static const struct i2c_device_id my_i2c_driver_ids[] = {
    // { 
    //     .compatible = "kingnan,i2c_dev", // 设备树匹配字符串; 厂家名称,芯片名字 : atmel,at24c02
    //     .data = NULL,// 芯片相关私有数据        (kernel_ulong_t)NULL, // 私有数据
    // },
    { "kingnan,i2c_dev",	(kernel_ulong_t)NULL },
    { /* sentinel */ }  
};


/**
 * 构造一个 my_i2c_driver
 * 
 */
static struct i2c_driver my_i2c_driver = {
    .driver = {
        .name = "my_i2c_driver",
        .of_match_table = of_match_ptr(of_match),
        // .acpi_match_table = ACPI_PTR(at24_acpi_match),
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

