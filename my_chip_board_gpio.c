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
#include "my_op.h"
#include "my_drv.h"



static int global_pins[100];
static int global_cnt = 0;



static int board_init(int which) {
    printk(KERN_INFO "board_init  %s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

    printk("init gpio: group %d, pin %d\n", GROUP(global_pins[which]), PIN(global_pins[which]));

    printk("board_init global_pins[which] : %d\n", global_pins[which]);
    // 这里写逻辑

    return 0;
}

static int board_ctl(int which, int status) {
    printk("set led %s: group %d, pin %d\n", status ? "on" : "off", GROUP(global_pins[which]), PIN(global_pins[which] ));

    printk("board_ctl global_pins[which] : %d\n", global_pins[which]);
    return 0;
}

// 读取
static int board_read(int which, char *status) {
    printk(KERN_INFO "%s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, which);
    char  gpio_value = 100;
    // unsigned int gpio_state;
    // gpio_state = *GPLEV0;
    if (copy_to_user(status, &gpio_value, 1)) {
        printk(KERN_ERR "Failed to copy GPIO status to user\n");
        return -EFAULT;
    }
    return 1;
}

// 退出
static int board_exit(int which) {
    printk(KERN_INFO "%s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, which);
    return 0;
}



static struct my_operations board_operations = {
    // .num = 1,
    .init = board_init,
    .ctl = board_ctl,
    .read = board_read,
    .exit = board_exit
};


struct my_operations * get_board_operations(void)
{
    return &board_operations;
}



/**
 * 
 * 当发现 platform_device 的 可以匹配的 时候就会执行 probe 函数
 * 1、记录引脚 ； 定义数组记录引脚 
 *      global_pins 里获得资源 ，从资源里获得 确定这个引脚
 * 2、 platform_get_resource(dev, IORESOURCE_IRQ, i++);  
 *          从 dev 中 获得 资源 ； 获得  flags = IORESOURCE_IRQ,  这一类资源； 获得 第 i 个  资源
 * 
 * 3、创建 device
 *    不能直接调用  device_create(); 因为这个函数在 my_drv.c 中， 所以要把 my_drv.c 中的 device_create(); 封装成一个函数
 *  
 * 
 */



static int my_chip_board_gpio_dirver_probe(struct platform_device *dev){
    printk(KERN_INFO "my_chip_board_gpio_dirver_probe \n");

    int i = 0;
    struct resource *res;
    while(1){
        res = platform_get_resource(dev, IORESOURCE_IRQ, i++); // 获取资源
        if(!res){
            break;
        }
        //记录引脚 
        global_pins[global_cnt] = res->start;
        // 2、device_create
        _device_create(global_cnt);// 调用 my_drv.c 中的 _device_create ，传入 次设备号
        global_cnt++;
    }

    return 0;
}

static int my_chip_board_gpio_dirver_remove(struct platform_device *dev){
    printk(KERN_INFO "my_chip_board_gpio_dirver_remove \n");

    // device_disroy
    // int i;
    // for(i = 0; i < global_cnt; i++){
    //     _device_destroy(i);
    // }
    // global_cnt = 0;


    struct resource *res;
    int i = 0;
    while (1)
    {
        res = platform_get_resource(dev, IORESOURCE_IRQ, i);
        if(!res){
            break;
        }
        if (i < global_cnt) {
            _device_destroy(i);
        }
        i++;
        // global_cnt--;
    }
    global_cnt = 0;
    return 0;
}



/**
 * 
 * 名字 跟 platform_device  
 * probe 函数;当实现配对就执行 probe 函数
 * remove 函数;把设备去掉使用 remove 函数
 * dirver = {
 *    .name = "my_board_n",// 名字 用来跟 platform_device 配对 如果配对成功
 * }
 * 
 */
static struct platform_driver my_chip_board_gpio_dirver = {
    .probe = my_chip_board_gpio_dirver_probe,
    .remove = my_chip_board_gpio_dirver_remove,
    .driver = {
        .name = "my_board_n",// 名字 用来跟 platform_device 配对 如果配对成功
    },
};


static int __init my_chip_board_gpio_dirver_init(void) {

    printk(KERN_INFO "my_chip_board_gpio_dirver_init \n");
    int err;
    err = platform_driver_register(&my_chip_board_gpio_dirver);

    _register_device_operations(&board_operations);
    return 0;
}

static void __exit my_chip_board_gpio_dirver_exit(void) {
    printk(KERN_INFO "my_chip_board_gpio_dirver_exit \n");
    platform_driver_unregister(&my_chip_board_gpio_dirver);
}

module_init(my_chip_board_gpio_dirver_init);
module_exit(my_chip_board_gpio_dirver_exit);
MODULE_LICENSE("GPL");


