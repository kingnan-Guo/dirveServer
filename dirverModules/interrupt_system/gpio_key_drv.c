#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>


struct gpio_key
{
    char name[100];// 名字
    int irq;// 中断号
    int cnt; // 计数器
};

static struct gpio_key gpio_keys[100];// gpio_key 数组




// gpio_key_isr
static irqreturn_t gpio_key_isr(int irq, void *dev_id){
    struct gpio_key *_gpio_key = dev_id;

    printk("gpio_key_isr %s cnt %d\n", _gpio_key->name, _gpio_key->cnt++);

    return IRQ_HANDLED;
}


static int gpio_key_probe(struct platform_device *pdev)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    int err;
    // struct device_node *node = pdev->dev.of_node;// 获得 设备树的节点
    int irq;

    int i = 0;
    while (1)
    {
        irq = platform_get_irq(pdev, i);// 获得中断号, 获取 第 i 个中断号
        if (irq < 0)
        {
            break;
        }

        gpio_keys[i].irq = irq;// 保存中断号
        snprintf(gpio_keys[i].name, sizeof(gpio_keys[i].name), "gpio_key%d", i);// 保存名字

        err = devm_request_irq(&pdev->dev, gpio_keys[i].irq, gpio_key_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, gpio_keys[i].name, &gpio_keys[i]);// 注册中断处理函数
        i++;
    }
    



    return 0;
}


static int gpio_key_remove(struct platform_device *pdev){
    return 0;
}


static const struct of_device_id ask100_keys[] = {
    { .compatible = "kingnan,virtual_gpio_keys" },
    { },
};

/* 1. 定义platform_driver */
static struct platform_driver gpio_keys_driver = {
    .probe      = gpio_key_probe,
    .remove     = gpio_key_remove,
    .driver     = {
        .name   = "virtual_gpio_keys",
        .of_match_table = ask100_keys,
    },
};




/* 2. 在入口函数注册platform_driver */
static int __init gpio_key_init(void)
{
    int err;
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
    err = platform_driver_register(&gpio_keys_driver); 
	
	return err;
}

/* 3. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 *     卸载platform_driver
 */
static void __exit gpio_key_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    platform_driver_unregister(&gpio_keys_driver);
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(gpio_key_init);
module_exit(gpio_key_exit);

MODULE_LICENSE("GPL");

