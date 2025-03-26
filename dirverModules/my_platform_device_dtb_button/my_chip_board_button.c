#include <linux/module.h>

#include <linux/fs.h>
#include <linux/io.h>
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
#include <asm/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>


#include "my_op.h"
#include "my_drv.h"

#define GROUP(X) (X >> 16)
#define PIN(X) (X&0xffff)
#define GROUP_PIN(group, pin) ((group << 16) | pin)


static int global_pins[100];
static int global_cnt = 0;

#define GPIO_PIN 27

#define BCM2837_PERI_BASE   0x3F000000
#define BCM2837_GPIO_BASE   (BCM2837_PERI_BASE + 0x200000)

struct rpi_gpio {
    volatile uint32_t GPFSEL[6];    // 0x00-0x14: 功能选择
    volatile uint32_t reserved1;    // 0x18: 保留
    volatile uint32_t GPSET[2];     // 0x1C-0x20: 输出置位
    volatile uint32_t reserved2;    // 0x24: 保留
    volatile uint32_t GPCLR[2];     // 0x28-0x2C: 输出清零
    volatile uint32_t reserved3;    // 0x30: 保留
    volatile uint32_t GPLEV[2];     // 0x34-0x38: 输入电平
    volatile uint32_t reserved4;    // 0x3C: 保留
    volatile uint32_t GPEDS[2];     // 0x40-0x44: 事件检测状态
    volatile uint32_t reserved5;    // 0x48: 保留
    volatile uint32_t GPREN[2];     // 0x4C-0x50: 上升沿检测使能
    volatile uint32_t reserved6;    // 0x54: 保留
    volatile uint32_t GPFEN[2];     // 0x58-0x5C: 下降沿检测使能
    volatile uint32_t reserved7;    // 0x60: 保留
    volatile uint32_t GPHEN[2];     // 0x64-0x68: 高电平检测使能
    volatile uint32_t reserved8;    // 0x6C: 保留
    volatile uint32_t GPLEN[2];     // 0x70-0x74: 低电平检测使能
    volatile uint32_t reserved9;    // 0x78: 保留
    // ... 其他寄存器按需扩展
};


static struct rpi_gpio *gpio;


static int board_init(int which) {
    printk(KERN_INFO "board_init  %s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

    printk("init gpio: group %d, pin %d\n", GROUP(global_pins[which]), PIN(global_pins[which]));

    printk("board_init global_pins[which] : %d\n", global_pins[which]);
    // 这里写逻辑

    // 映射 GPIO 寄存器
    gpio = ioremap(BCM2837_GPIO_BASE, sizeof(struct rpi_gpio));
    if(!gpio){
        printk(KERN_ERR "Failed to map GPIO registers\n");
        return -ENOMEM;
    }

    // uint32_t reg = gpio->GPFSEL[2];

    int reg_index = GPIO_PIN / 10;
    int bit_offset = (GPIO_PIN % 10) * 3;

    gpio->GPFSEL[reg_index] &= ~(0x07 << bit_offset);// 000 = 输入模式 

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
    // char  gpio_value = 100;
    // unsigned int gpio_state;
    // gpio_state = *GPLEV0;
    // if (copy_to_user(status, &gpio_value, 1)) {
    //     printk(KERN_ERR "Failed to copy GPIO status to user\n");
    //     return -EFAULT;
    // }


    // char  gpio_value = 100;
    // unsigned int gpio_state;
    // gpio_state = 1;

    // // gpio_state = *GPLEV0; // GPLEV0 寄存器包含所有的 GPIO 引脚的 当前状态
    // // 读取 27 位的状态
    // if(gpio_state & (1 << GPIO_PIN)){
    //     gpio_value = '1';
    // } else {
    //     gpio_value = '0';
    // }

    // return gpio_value;

    unsigned int gpio_state;
    if (!gpio) {
        printk(KERN_ERR "Failed to map GPIO registers\n");
        return -ENOMEM;
    }

    gpio_state = gpio->GPLEV[GPIO_PIN / 32];

    *status = (gpio_state & (1 << GPIO_PIN % 32)) ? '1' : '0';

    printk(KERN_INFO "GPIO %d  static %c \n", GPIO_PIN, *status);
    return *status;




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
 * 每一次 dtb 节点 都会调用一次 probe 函数， 所以把 while 去掉
 * 
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



static int my_chip_board_gpio_dirver_probe(struct platform_device *pdev){
    printk(KERN_INFO "my_chip_board_gpio_dirver_probe \n");

    // int i = 0;
    // struct resource *res;

    // 要从 platform_device  pdev 中获取资源

    struct device_node *np;

    //  在 platform_device 中找到 dev 就是  device 描述设备信息， device 的 device_node  对应 到 of_node ， of_node 里储存的是 dtb 中的节点
    np = pdev->dev.of_node;

    int err = 0;
    int my_pin = 0;

    // 和这个 platform_driver 支持的  platform_device ， 可能来自设备树，也可能不是来自设备树 ，所以要判断
    if(!np){
        return -ENODEV;
    }

    // 1、获取资源
    // 从 np 中获取 pin， 把值 储存到 global_pins 中
    err = of_property_read_u32(np, "pin", &my_pin);

    // res = platform_get_resource(pdev, IORESOURCE_IRQ, i++); // 获取资源
    // if(!res){
    //     break;
    // }

    //记录引脚 
    global_pins[global_cnt] = my_pin;
    // 2、device_create
    _device_create(global_cnt);// 调用 my_drv.c 中的 _device_create ，传入 次设备号
    global_cnt++;

    return 0;
}

static int my_chip_board_gpio_dirver_remove(struct platform_device *pdev){
    printk(KERN_INFO "my_chip_board_gpio_dirver_remove \n");


    int i = 0;
    struct device_node *np;
    int err = 0;
    int my_pin = 0;

    np = pdev->dev.of_node;
    if (!np)
        return -1;

    // 从 np 中获取 pin， 把值 储存到 global_pins 中
    err = of_property_read_u32(np, "pin", &my_pin);

    for (i = 0; i< global_cnt; i++)
    {

        if (global_pins[i] == my_pin) {
            _device_destroy(i);
            global_pins[i]  = -1;// 
            break;
        }
        // global_cnt--;
    }

    for (i = 0; i < global_cnt; i++)
    {
        if(global_pins[i] != -1){
            break;
        }

    }
    if(i == global_cnt){
        global_cnt = 0;
    }
    
    return 0;

}



/* ============================================================= */

static const struct of_device_id my_chip_board_gpio_of_match[] = {
    { .compatible = "my_board_device,my_drv" },
    { /* sentinel */ }
};

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
        .of_match_table = my_chip_board_gpio_of_match
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
    _register_device_operations(NULL); // 清空指针

}

module_init(my_chip_board_gpio_dirver_init);
module_exit(my_chip_board_gpio_dirver_exit);
MODULE_LICENSE("GPL");