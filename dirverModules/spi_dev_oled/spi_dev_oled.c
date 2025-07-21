#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/acpi.h>

#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include <linux/uaccess.h>
#include <linux/gpio/consumer.h>


//为0 表示命令，为1表示数据
#define OLED_CMD 	0
#define OLED_DATA 	1

#define OLED_IOC_INIT 			123
#define OLED_IOC_SET_POS 		124


static struct  spi_device *oled_spi;
static int major;
static struct gpio_desc *dc_gpio;



void dc_pin_init(void) {
    //初始化 dc 引脚
    // 设置 dc 为 输出
    gpiod_direction_output(dc_gpio, 1); // 默认拉高. 设置 为 输出

}

void oled_set_dc_pin(int val){
    // 设置 DC 引脚
    gpiod_set_value(dc_gpio, val); // 设置 DC 引脚的值 0 或 1

}
void spi_write_datas(unsigned char *buf, int len){
    spi_write(oled_spi, buf, len); // 通过 SPI 写入数据
}



// 写数据
void oled_write_cmd_data(unsigned char uc_data, unsigned char uc_cmd){
    if(uc_cmd == OLED_CMD){
        oled_set_dc_pin(0); // 设置为命令模式
    }
    else {
        oled_set_dc_pin(1); // 设置 写入数据
    }

    spi_write_datas(&uc_data, 1); // 写入数据
    
}

// 初始化 oled
static int oled_init(void){
	oled_write_cmd_data(0xae,OLED_CMD);//关闭显示

	oled_write_cmd_data(0x00,OLED_CMD);//设置 lower column address
	oled_write_cmd_data(0x10,OLED_CMD);//设置 higher column address
	oled_write_cmd_data(0x40,OLED_CMD);//设置 display start line
	oled_write_cmd_data(0xB0,OLED_CMD);//设置page address
	oled_write_cmd_data(0x81,OLED_CMD);// contract control
	oled_write_cmd_data(0x66,OLED_CMD);//128
	oled_write_cmd_data(0xa1,OLED_CMD);//设置 segment remap
	oled_write_cmd_data(0xa6,OLED_CMD);//normal /reverse
	oled_write_cmd_data(0xa8,OLED_CMD);//multiple ratio
	oled_write_cmd_data(0x3f,OLED_CMD);//duty = 1/64
	oled_write_cmd_data(0xc8,OLED_CMD);//com scan direction
	oled_write_cmd_data(0xd3,OLED_CMD);//set displat offset
	oled_write_cmd_data(0x00,OLED_CMD);//
	oled_write_cmd_data(0xd5,OLED_CMD);//set osc division
	oled_write_cmd_data(0x80,OLED_CMD);//
	oled_write_cmd_data(0xd9,OLED_CMD);//ser pre-charge period
	oled_write_cmd_data(0x1f,OLED_CMD);//
	oled_write_cmd_data(0xda,OLED_CMD);//set com pins
	oled_write_cmd_data(0x12,OLED_CMD);//
	oled_write_cmd_data(0xdb,OLED_CMD);//set vcomh
	oled_write_cmd_data(0x30,OLED_CMD);//
	oled_write_cmd_data(0x8d,OLED_CMD);//set charge pump disable 
	oled_write_cmd_data(0x14,OLED_CMD);//
	oled_write_cmd_data(0xaf,OLED_CMD);//set dispkay on

	return 0;
}






static ssize_t
spidev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
    char* kern_buf;
    kern_buf = kmalloc(count, GFP_KERNEL);// 分配内存
    if (kern_buf == NULL) {
        return -ENOMEM; // 内存分配失败
    }
    int err = copy_from_user(kern_buf, buf, count); // 从用户空间复制数据到内核空间

    // 设置 写入数据
    oled_set_dc_pin(1);// 拉高 DC 引脚，表示写入数据
    spi_write_datas(kern_buf, count); // 通过 SPI 写入数据
    kfree(kern_buf);
	return count;
}


void OLED_DIsp_Set_Pos(int x, int y){
    // 
    oled_write_cmd_data(0xb0 + y, OLED_CMD); // 设置页地址第几页
    oled_write_cmd_data((x&0x0f), OLED_CMD); // 设置列地址低位;  0x00 到 0x0f 用于低 4 位。
    oled_write_cmd_data((x&0xf0)>>4 | 0x10, OLED_CMD); // 设置列地址高位  ; 0x10 到 0x1f 用于高 4 位。

}


static long
spidev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int x, y;
    //  根据 cmd 操作 硬件
    switch (cmd)
    {
        case OLED_IOC_INIT: // 初始化 OLED
            dc_pin_init();// 初始化 DC 引脚
            oled_init();// 初始化 OLED
            break;
        case OLED_IOC_SET_POS: // 设置位置
            x = arg & 0xff; // 获取 x 坐标
            y = (arg >> 8) & 0xff; // 获取 y 坐标
            OLED_DIsp_Set_Pos(x, y); // 设置位置

            break;
        default:
            break;
    } 


	return 0;
}





static int spidev_release(struct inode *inode, struct file *filp)
{
	// struct spidev_data	*spidev;
	return 0;
}

static const struct file_operations spidev_fops = {
	.owner =	THIS_MODULE,
	/* REVISIT switch to aio primitives, so that userspace
	 * gets more complete API coverage.  It'll simplify things
	 * too, except for the locking.
	 */
	.write =	spidev_write,
	// .read =		spidev_read,
	.unlocked_ioctl = spidev_ioctl,
	// .compat_ioctl = spidev_compat_ioctl, // 这个 是 32 位程序调用 64 位的 ioctl
	.release =	spidev_release,
};

/*-------------------------------------------------------------------------*/

/* The main reason to have this class is to make mdev/udev create the
 * /dev/spidevB.C character device nodes exposing our userspace API.
 * It also simplifies memory management.
 */




static struct class *spidev_class;



static const struct of_device_id spidev_dt_ids[] = {
	{ .compatible = "spi_dev_oled" },
	{},
};
MODULE_DEVICE_TABLE(of, spidev_dt_ids);




/*-------------------------------------------------------------------------*/

static int spidev_probe(struct spi_device *spi)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    /* 1 记录 spi dev */
    oled_spi = spi;

    /* 2 注册字符设备 */
    major  = register_chrdev(0, "spi_dev_oled", &spidev_fops);
    spidev_class = class_create( "spi_dev_oled");
    device_create(spidev_class, NULL, MKDEV(major, 0), NULL, "spi_dev_oled");
    
    /* 3  获取 dc 引脚， 从设备树里面 获取    dc-gpio = <&gpio 12 GPIO_ACTIVE_HIGH>; */
    dc_gpio = gpiod_get(&spi->dev, "dc", 0);

	return 0;
}

static void spidev_remove(struct spi_device *spi)
{
	// struct spidev_data	*spidev = spi_get_drvdata(spi);

	/* 1 注销字符设备 */
    device_destroy(spidev_class, MKDEV(major, 0));
    class_destroy(spidev_class);
	unregister_chrdev(major, "spi_dev_oled");
    /* 2 释放 spi dev */
    gpiod_put(dc_gpio); // 释放 dc 引脚
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

}

static struct spi_driver spidev_spi_driver = {
	.driver = {
		.name =		"spidev,spi_dev_oled",
		.of_match_table = spidev_dt_ids,
	},
	.probe =	spidev_probe,
	.remove =	spidev_remove,
	// .id_table =	spidev_spi_ids,

	/* NOTE:  suspend/resume methods are not necessary here.
	 * We don't do anything except pass the requests to/from
	 * the underlying controller.  The refrigerator handles
	 * most issues; the controller driver handles the rest.
	 */
};

/*-------------------------------------------------------------------------*/

static int __init spidev_init(void)
{
	int status;

	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	status = spi_register_driver(&spidev_spi_driver);
	if (status < 0) {

	}
	return status;
}
module_init(spidev_init);

static void __exit spidev_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	spi_unregister_driver(&spidev_spi_driver);

}
module_exit(spidev_exit);


MODULE_LICENSE("GPL");
MODULE_ALIAS("spi:spidev");


