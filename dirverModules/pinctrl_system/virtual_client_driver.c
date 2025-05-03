#include <linux/module.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/slab.h>
#include <linux/regmap.h>

// virtual_client_driver_probe
static int virtual_client_driver_probe(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "virtual_client_driver_probe\n");

    // struct pinctrl_desc *pictrl;

    return 0;
}

static int virtual_client_driver_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "virtual_client_driver_remove\n");
    /* 1.1 反注册platform_driver */
    // platform_driver_unregister(&virtual_controller_driver_device_driver);
    return 0;
}

// virtual_client_driver_of_match
static const struct of_device_id virtual_client_driver_of_match[] = {
    { .compatible = "kingnan,virtual_i2c", },
    { },
};

static struct platform_driver virtual_client_driver_device_driver = {
    .probe      = virtual_client_driver_probe,
    .remove     = virtual_client_driver_remove,
    .driver     = {
        .name   = "virtual_client_driver",
        .of_match_table = of_match_ptr(virtual_client_driver_of_match),
    }
};

static int __init virtual_client_driver_init(void)
{
    int ret;

    ret = platform_driver_register(&virtual_client_driver_device_driver);
    if (ret) {
        pr_err("Failed to register virtual_client_driver: %d\n", ret);
        return ret;
    }
    return 0;
}

static void __exit virtual_client_driver_exit(void)
{
    platform_driver_unregister(&virtual_client_driver_device_driver);
}

module_init(virtual_client_driver_init);
module_exit(virtual_client_driver_exit);

MODULE_LICENSE("GPL");