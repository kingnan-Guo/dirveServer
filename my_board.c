#include "my_op.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/gfp.h>

// 初始化 LED，which 代表哪个 LED
static int board_init(int which) {
    printk(KERN_INFO "%s %s line %d  led  %d \n", __FILE__, __FUNCTION__, __LINE__, which);
    return 0;
}

// 控制 LED，which 代表哪个 LED，status 1 表示打开，0 表示关闭
static int board_ctl(int which, char status) {
    printk(KERN_INFO "%s %s line %d  led  %d , %s \n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
    return 0;
}

// 修正 `my_operations` 结构体的定义，确保加上 `struct`
static struct my_operations board_operations = {
    .init = board_init,
    .ctl = board_ctl,
};

// 返回 `board_operations` 结构体指针
struct my_operations *my_board_operations(void) {
    return &board_operations;
}

MODULE_LICENSE("GPL");