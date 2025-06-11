#include <linux/completion.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c-algo-bit.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_data/i2c-gpio.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/slab.h>



static struct i2c_adapter *global_i2c_adapter;

static unsigned char eeprom_buffer[512]; // 模拟 EEPROM 的缓冲区
static int eeprom_currect_address = 0; // 模拟 EEPROM 的当前地址

//  模拟 EEPROM 的传输
static void eeprom_emulate_xfer(struct i2c_adapter *adapter, struct i2c_msg *msgs){
    int i;
    if(msgs->flags & I2C_M_RD){
        printk(KERN_INFO "eeprom_emulate_xfer: read\n");
        for ( i = 0; i < msgs->len; i++)
        {
           msgs->buf[i] = eeprom_buffer[eeprom_currect_address++]; // 从缓冲区读取数据
           if(eeprom_currect_address == 512){
               eeprom_currect_address = 0;
           }
        }
        
    }else{
        printk(KERN_INFO "eeprom_emulate_xfer: write\n");

        if(msgs->len > 0){
            eeprom_currect_address = msgs->buf[0]; // 设置当前地址,为什么 msg->buf[0] 是地址呢，因为 i2c_msg 的数据是 2 个字节，第一个字节是地址，第二个字节是数据
            for ( i = 1; i < msgs->len; i++)
            {
                /* code */
                eeprom_buffer[eeprom_currect_address++] = msgs->buf[i]; // 将数据写入缓冲区
                if(eeprom_currect_address == 512){
                    eeprom_currect_address = 0; // 如果超过了缓冲区的大小，就从头开始
                }
            }
            
        }
    }
}

// i2c_bus_virtual_master_xfer
// 要传输  n 个 i2c_msg
// 这个函数是 i2c_adapter 的传输函数
static int i2c_bus_virtual_master_xfer(struct i2c_adapter *adapter, struct i2c_msg msgs[], int num){
	int i;

	for (i = 0; i < num; i++)
	{
		// do transfer msgs[i];
        if(msgs[i].addr == 0x60){
            printk(KERN_INFO "i2c_bus_virtual: master_xfer\n");
            eeprom_emulate_xfer(adapter, &msgs[i]);

        } else{
            i = -EIO; // 如果不是 0x50 的地址，返回错误
            break;
        }
	}
	return i;
}


static u32 i2c_bus_virtual_func(struct i2c_adapter *adapter){
    // 
	return I2C_FUNC_I2C // I2C_FUNC_I2C 是 I2C 协议的功能
        | I2C_FUNC_NOSTART // I2C_FUNC_NOSTART 是不需要起始信号的功能
        | I2C_FUNC_SMBUS_EMUL // I2C_FUNC_SMBUS_EMUL 是 SMBus 协议的功能
        | I2C_FUNC_SMBUS_READ_BLOCK_DATA // I2C_FUNC_SMBUS_READ_BLOCK_DATA 是 SMBus 读取块数据的功能
        | I2C_FUNC_SMBUS_BLOCK_PROC_CALL // I2C_FUNC_SMBUS_BLOCK_PROC_CALL 是 SMBus 块过程调用的功能
        | I2C_FUNC_PROTOCOL_MANGLING;// I2C_FUNC_PROTOCOL_MANGLING 是协议篡改的功能

}



struct i2c_algorithm i2c_bus_virtual_algo = {
	.master_xfer	= i2c_bus_virtual_master_xfer,// i2c_adapter 的传输函数
	.functionality	= i2c_bus_virtual_func,// i2c_adapter 的功能函数
};


static int i2c_bus_virtual_probe(struct platform_device *pdev)
{
    /** 从设备树里 获取信息， 用来设置  i2c_adapter/hardware */
	/** alloc   set     register  :  i2c_adpter */
	struct device *dev = &pdev->dev;
    global_i2c_adapter = devm_kzalloc(dev, sizeof(*global_i2c_adapter), GFP_KERNEL);


	global_i2c_adapter->owner = THIS_MODULE;// i2c_adapter 的拥有者
	global_i2c_adapter->class = I2C_CLASS_HWMON | I2C_CLASS_SPD;// i2c_adapter 的类型 ，用来分类设备 I2C_CLASS_HWMON 是 硬件监控设备 I2C_CLASS_SPD 是 spd设备

	global_i2c_adapter->nr = -1;// i2c_adapter 的编号 让系统分配

	global_i2c_adapter->algo = &i2c_bus_virtual_algo;// i2c_adapter 的算法，用来和从设备通信



    /** 设置 i2c_adapter 的名字 */
    snprintf(global_i2c_adapter->name, sizeof(global_i2c_adapter->name), "i2c-bus-virtual");
    // 注册
	int ret = i2c_add_adapter(global_i2c_adapter);// 如果确定 nr 的话 使用 i2c_add_numbered_adapter(global_i2c_adapter);

    if (ret) {
        dev_err(dev, "Failed to add i2c adapter: %d\n", ret);
        return ret;
    }

    dev_info(dev, "Virtual I2C adapter registered\n");

	return 0;
}

static int i2c_bus_virtual_remove(struct platform_device *pdev)
{
	i2c_del_adapter(global_i2c_adapter);
    global_i2c_adapter = NULL; // 清空全局变量
    printk(KERN_INFO "i2c_bus_virtual: removed\n");
    return 0;

}

static const struct of_device_id of_match[] = {
    { .compatible = "kingnan,i2c-bus-virtual", .data = NULL },
    { /* END OF LIST */ },
};


static struct platform_driver i2c_bus_virtual_driver = {
	.driver		= {
		.name	= "i2c-bus-virtual",
		.of_match_table	= of_match,

	},
	.probe		= i2c_bus_virtual_probe,
	.remove	= i2c_bus_virtual_remove,
};

static int __init i2c_bus_virtual_init(void)
{
	int ret;

	ret = platform_driver_register(&i2c_bus_virtual_driver);
	if (ret)
		printk(KERN_ERR "i2c_bus_virtual: probe failed: %d\n", ret);

	return ret;
}
module_init(i2c_bus_virtual_init);

static void __exit i2c_bus_virtual_exit(void)
{
	platform_driver_unregister(&i2c_bus_virtual_driver);
}
module_exit(i2c_bus_virtual_exit);

MODULE_AUTHOR("kingnan");
MODULE_LICENSE("GPL v2");
