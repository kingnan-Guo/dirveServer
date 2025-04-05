
#include <linux/module.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>



 static int input_device_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;

    return 0;
}

static int input_device_remove(struct platform_device *pdev)
{

    return 0;
}

static void input_device_shutdown(struct platform_device *pdev)
{

}

 static const struct of_device_id input_device_of_match[] = {
    { .compatible = "dirve,input_device", },
    { },
};


static struct platform_driver input_device_device_driver = {
    .probe      = input_device_probe,
    .shutdown   = input_device_shutdown,
    .remove     = input_device_remove,
    .driver     = {
        .name   = "input_device",
        .of_match_table = input_device_of_match,
    }
};



static int __init input_device_init(void)
{
    return platform_driver_register(&input_device_device_driver);
}

static void __exit input_device_exit(void)
{
    platform_driver_unregister(&input_device_device_driver);
}

late_initcall(input_device_init);
module_exit(input_device_exit);

MODULE_LICENSE("GPL");


