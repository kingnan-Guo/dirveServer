#include "my_op.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/gfp.h>
#include <linux/io.h>


#define BCM2837_PERI_BASE   0x3F000000
#define BCM2837_GPIO_BASE   (BCM2837_PERI_BASE + 0x200000)

#define GPFSEL0_OFFSET      0x00
#define GPFSEL1_OFFSET      0x04
#define GPFSEL2_OFFSET      0x08
#define GPSET0_OFFSET       0x1C
#define GPCLR0_OFFSET       0x28
#define GPLEV0_OFFSET       0x34

static volatile unsigned int *GPIO_BASE = NULL;

#define GPFSEL0 ((volatile unsigned int *)(GPIO_BASE + (GPFSEL0_OFFSET / 4)))
#define GPFSEL1 ((volatile unsigned int *)(GPIO_BASE + (GPFSEL1_OFFSET / 4)))
#define GPFSEL2 ((volatile unsigned int *)(GPIO_BASE + (GPFSEL2_OFFSET / 4)))
#define GPSET0  ((volatile unsigned int *)(GPIO_BASE + (GPSET0_OFFSET / 4)))
#define GPCLR0  ((volatile unsigned int *)(GPIO_BASE + (GPCLR0_OFFSET / 4)))
#define GPLEV0  ((volatile unsigned int *)(GPIO_BASE + (GPLEV0_OFFSET / 4))) // 读取 gpio 的状态


static int GPIO_PIN = 27;


// 初始化 LED，which 代表哪个 LED
// 从代码上来看 which 好像是 次设备号
static int board_init(int which) {
    printk(KERN_INFO "%s %s line %d  led  %d \n", __FILE__, __FUNCTION__, __LINE__, which);


    GPIO_BASE = ioremap(BCM2837_GPIO_BASE, 0x1000);
    if (!GPIO_BASE) {
        printk(KERN_ERR "Failed to map GPIO registers\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "GPIO_BASE mapped to: %p\n", GPIO_BASE);
    printk(KERN_INFO "GPFSEL2 address: %p, value: 0x%x\n", GPFSEL2, *GPFSEL2);

    // 
    if(which == 0){

        unsigned int gpfsel2_val; // 用来存储 GPFSEL2 的值
        printk(KERN_INFO "GPFSEL2 before: 0x%x\n", *GPFSEL2);
        gpfsel2_val = *GPFSEL2;
        // 清除 21-23 位
        gpfsel2_val &= ~(0x07 << 21);
        // 设置 21-23 位
        gpfsel2_val |= (0x01 << 21);

        *GPFSEL2 = gpfsel2_val;
        printk(KERN_INFO "GPFSEL2 after: 0x%x\n", *GPFSEL2);

    }

    return 0;
}

// 控制 LED，which 代表哪个 LED，status 1 表示打开，0 表示关闭
static int board_ctl(int which, int status) {
    printk(KERN_INFO "%s %s line %d  led  %d  status %s \n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
    
    if(which == 0){
        // char value;

        if(status){
            printk(KERN_INFO "Turning LED ON\n");
            *GPSET0 = (1 << GPIO_PIN);
            printk(KERN_INFO "GPSET0 after: 0x%x\n", *GPSET0);
        } else {
            printk(KERN_INFO "Turning LED OFF\n");
            *GPCLR0 = (1 << GPIO_PIN);
            printk(KERN_INFO "GPCLR0 after: 0x%x\n", *GPCLR0);
        }
    }
    
    return 0;
}

static int board_exit(int which) {
    printk(KERN_INFO "%s %s line %d board_exit %d\n", __FILE__, __FUNCTION__, __LINE__, which);

    if (GPIO_BASE) {
        iounmap(GPIO_BASE);
        GPIO_BASE = NULL;
        printk(KERN_INFO "GPIO_BASE unmapped\n");
    }
    return 0;
}

// read
static int board_read(int which, char *status) {
    
    printk(KERN_INFO "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    // 读取 gpio 的状态
    char  gpio_value;
    unsigned int gpio_state;

    gpio_state = *GPLEV0; // GPLEV0 寄存器包含所有的 GPIO 引脚的 当前状态
    // 读取 27 位的状态
    if(gpio_state & (1 << GPIO_PIN)){
        gpio_value = '1';
    } else {
        gpio_value = '0';
    }

    if (copy_to_user(status, &gpio_value, 1)) {
        printk(KERN_ERR "Failed to copy GPIO status to user\n");
        return -EFAULT;
    }

    // if(copy_to_user(buffer, &gpio_value, 1)){
    //     return -EFAULT;
    // }
    
    printk(KERN_INFO "GPIO %d  static %c \n", GPIO_PIN, gpio_value);

    
    
    return 1;
}

// 修正 `my_operations` 结构体的定义，确保加上 `struct`
static struct my_operations board_operations = {
    .num = 1,
    .init = board_init,
    .ctl = board_ctl,
    .read = board_read,
    .exit = board_exit
};

// 返回 `board_operations` 结构体指针
struct my_operations *get_board_operations(void) {
    return &board_operations;
}

MODULE_LICENSE("GPL");