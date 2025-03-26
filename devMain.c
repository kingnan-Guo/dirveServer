#include <linux/module.h>
#include <linux/kernel.h>

static int my_driver_init(void) {
    printk(KERN_INFO "Initializing devMain\n");
    return -1;
}

static void my_driver_exit(void) {

    printk(KERN_INFO "Cleaning up devMain\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("GPIO Interrupt Driver for Raspberry Pi");