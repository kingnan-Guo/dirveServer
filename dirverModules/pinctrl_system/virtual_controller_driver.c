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
#include <linux/virtual_controller_driver.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>
#include <dt-bindings/input/gpio-keys.h>

static struct pinctrl_dev *global_pinctrl_dev;

// virtual_controller_driver_probe
static int virtual_controller_driver_probe(struct platform_device *pdev){
    dev_info(&pdev->dev, "virtual_controller_driver_probe\n");

    struct pinctrl_desc *pictrl;

    /** 1 分配  pinctrl_desc */
    pictrl = devm_kzalloc(&pdev->dev, sizeof(*pinctrl), GFP_KERNEL);



    /** 2 设置  pinctrl_desc */
    
    // 设置name
    pictrl->name = dev_name(&pdev->dev);
    pictrl->owner = THIS_MODULE;
    pictrl->pins = pins;//支持 哪些 引脚
    pictrl->npins = ARRAY_SIZE(pins);



    /** 3 注册  pinctrl_desc */
    global_pinctrl_dev = devm_pinctrl_register(
        &pdev->dev, pictrl, NULL
    ); 
    

    return 0;
}


// virtual_controller_driver_of_match
static const struct of_device_id virtual_controller_driver_of_match[] = {
    { .compatible = "kingnan,virtual_controller_driver", },
    { },
}

static void virtual_controller_driver_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "virtual_controller_driver_remove\n");
    /* 1.1 反注册platform_driver */
    // platform_driver_unregister(&virtual_controller_driver_device_driver);
}

static struct platform_driver virtual_controller_driver_device_driver = {
	.probe		= virtual_controller_driver_probe,
	.remove	= virtual_controller_driver_remove,
	.driver		= {
		.name	= "virtual_controller_driver",
		.of_match_table = virtual_controller_driver_of_match,
	}
};

static int __init virtual_controller_driver_init(void)
{
	return platform_driver_register(&virtual_controller_driver_device_driver);
}

static void __exit virtual_controller_driver_exit(void)
{
	platform_driver_unregister(&virtual_controller_driver_device_driver);
}

module_init(virtual_controller_driver_init);
module_exit(virtual_controller_driver_exit);

MODULE_LICENSE("GPL");