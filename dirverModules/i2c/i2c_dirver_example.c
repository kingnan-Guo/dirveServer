

#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/property.h>
#include <linux/acpi.h>
#include <linux/i2c.h>
#include <linux/nvmem-provider.h>
#include <linux/regmap.h>
#include <linux/pm_runtime.h>
#include <linux/gpio/consumer.h>





static const struct of_device_id of_match_ids_example[] = {
    { 
        .compatible = "com_name,chip_name", // 设备树匹配字符串; 厂家名称,芯片名字 : atmel,at24c02
        .data = NULL,// 芯片相关私有数据
    },
    { /* sentinel */ }  
}



static const struct i2c_device_id example_ids[] = {
    {
        "chip_name", // i2c设备名字
        (kernel_ulong_t)NULL, // 私有数据
    },
    { /* sentinel */ }
}


static int i2c_driver_example_probe(struct i2c_client *client) {
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static int i2c_driver_example_remove(struct i2c_client *client) {
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}


/**
 * 构造一个 i2c_driver
 * 
 */
static struct i2c_driver i2c_example_driver = {
    .driver = {
        .name = "example",
        .of_match_table = of_match_ptr(of_match),
        // .acpi_match_table = ACPI_PTR(at24_acpi_match),
    },
    .probe_new = i2c_driver_example_probe,
    .remove = i2c_driver_example_remove,
    .id_table = example_ids,
}




/* 1. 入口函数 */
static int __init i2c_dirver_example_init(void) {
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    /* 1.1 注册一个i2c_driver */
    return i2c_add_driver(&i2c_example_driver);
}

/* 2. 出口函数 */
static void __exit i2c_dirver_example_exit(void) {
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    /* 2.1 反注册i2c_driver */
    i2c_del_driver(&i2c_example_driver);
}

module_init(i2c_dirver_example_init);
module_exit(i2c_dirver_example_exit);

MODULE_AUTHOR("Your Name");
MODULE_LICENSE("GPL");
