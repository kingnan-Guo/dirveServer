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
#include <linux/platform_device.h>

#include "my_resources.h"


// static struct my_resources my_board_n = {
//     .pin = GROUP_PIN(3, 1)
// };

// struct my_resources * get_resources(void)
// {
//     return &my_board_n;
// }




// 资源 数组
// 要把资源 告诉 platform_device , 放到 resources 中
static struct resource resources[] = {
    {
        .start = GROUP_PIN(3, 1),// 表示资源的 起始值，具体含义取决于 flags ;  IRQ 号、GPIO 号、内存地址、I/O 端口
        //表示 哪一类资源  有  IORESOURCE_IRQ 中断  IORESOURCE_MEM 地址 IORESOURCE_IO 端口
        .flags = IORESOURCE_IRQ, // 可以借助 IORESOURCE_IRQ 来表示 引脚 ？？？
        .name = "my_board_pin",
    },
    {
        .start = GROUP_PIN(5, 8),
        //表示 哪一类资源  有  IORESOURCE_IRQ 中断  IORESOURCE_MEM 地址 IORESOURCE_IO 端口
        .flags = IORESOURCE_IRQ, // 可以借助 IORESOURCE_IRQ 来表示 引脚 ？？？
        .name = "my_board_pin",
    }

};



// 设备的释放函数
static void my_device_release(struct device *dev)
{
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    // 释放函数
    // printk(KERN_INFO "my_board_n ==   my_device_release  \n");
}

// 平台的 设备信息 用于注册设备， 类似于 dtb 
// 有platform_device 的时候 就要 有 对应的 platform_driver
static struct platform_device my_board_n_dev = {
    .name = "my_board_n",// 平台设备的 名称
    .num_resources = ARRAY_SIZE(resources),// 资源的个数 , ARRAY_SIZE 数组的个数
    .resource = resources,
    .dev = {
        .release = my_device_release, // 添加 设备的释放函数
    },
};


// 要注册这个结构体
static int __init my_board_n_init(void)
{
    int err;
    err = platform_device_register(&my_board_n_dev);
    return 0;
}


// 出口函数
static void __exit my_board_n_exit(void){
    platform_device_unregister(&my_board_n_dev);
}


module_init(my_board_n_init);
module_exit(my_board_n_exit);
MODULE_LICENSE("GPL");

