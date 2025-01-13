#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

// 模块加载时执行的函数
static int __init hello_world_init(void) {
    printk(KERN_INFO "Hello, World module loaded.\n");
    return 0;
}

// 模块卸载时执行的函数
static void __exit hello_world_exit(void) {
    printk(KERN_INFO "Hello, World module unloaded.\n");
}

// 定义模块的加载和卸载函数
module_init(hello_world_init);
module_exit(hello_world_exit);

