#include <linux/module.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/consumer.h>

// virtual_client_driver_probe
static int virtual_client_driver_probe(struct platform_device *pdev)
{
    struct pinctrl *pinctrl;
    struct pinctrl_state *state;

    dev_info(&pdev->dev, "virtual_client_driver_probe\n");

    // 获取 pinctrl
    pinctrl = devm_pinctrl_get(&pdev->dev);
    if (IS_ERR(pinctrl)) {
        dev_err(&pdev->dev, "Failed to get pinctrl: %ld\n", PTR_ERR(pinctrl));
        return PTR_ERR(pinctrl);
    }

    // 查找 default 状态
    state = pinctrl_lookup_state(pinctrl, PINCTRL_STATE_DEFAULT);
    if (IS_ERR(state)) {
        dev_err(&pdev->dev, "Failed to lookup default state: %ld\n", PTR_ERR(state));
        return PTR_ERR(state);
    }

    // 应用 pinctrl 配置
    if (pinctrl_select_state(pinctrl, state)) {
        dev_err(&pdev->dev, "Failed to select default state\n");
        return -EINVAL;
    }

    dev_info(&pdev->dev, "Applied pinctrl configuration\n");

    return 0;
}

static int virtual_client_driver_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "virtual_client_driver_remove\n");
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
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Virtual I2C client driver for pinctrl testing");