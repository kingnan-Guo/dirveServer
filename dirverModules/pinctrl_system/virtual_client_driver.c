#include <linux/module.h>

#include <linux/hrtimer.h>
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
#include <linux/virtual_client_driver.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>
#include <dt-bindings/input/gpio-keys.h>



// virtual_client_driver_probe
static int virtual_client_driver_probe(struct platform_device *pdev){
    dev_info(&pdev->dev, "virtual_client_driver_probe\n");

    struct pinctrl_desc *pictrl;

    return 0;
}


// virtual_client_driver_of_match
static const struct of_device_id virtual_client_driver_of_match[] = {
    { .compatible = "kingnan,virtual_client_driver", },
    { },
}

static void virtual_client_driver_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "virtual_client_driver_remove\n");
    /* 1.1 反注册platform_driver */
    // platform_driver_unregister(&virtual_client_driver_device_driver);
}

static struct platform_driver virtual_client_driver_device_driver = {
	.probe		= virtual_client_driver_probe,
	.remove	= virtual_client_driver_remove,
	.driver		= {
		.name	= "virtual_client_driver",
		.of_match_table = virtual_client_driver_of_match,
	}
};

static int __init virtual_client_driver_init(void)
{
	return platform_driver_register(&virtual_client_driver_device_driver);
}

static void __exit virtual_client_driver_exit(void)
{
	platform_driver_unregister(&virtual_client_driver_device_driver);
}

module_init(virtual_client_driver_init);
module_exit(virtual_client_driver_exit);

MODULE_LICENSE("GPL");
