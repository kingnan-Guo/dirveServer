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

#include <linux/fb.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>


//为0 表示命令，为1表示数据
#define OLED_CMD 	0
#define OLED_DATA 	1

#define OLED_IOC_INIT 			123
#define OLED_IOC_SET_POS 		124


static struct  spi_device *oled_spi;
static int major;
static struct gpio_desc *dc_gpio;

static struct fb_info *myfb_info; // framebuffer 的信息
static unsigned int pseudo_palette[16];// 伪调色板

static struct task_struct *oled_thread;// OLED 线程
static unsigned char *oled_buf; //[1024]; 这个是 OLED 的缓冲区； 1024 是。128 * 8 = 1024 字节； 128 列， 8 行




// 不知道 在做什么 --------------------------------

// 静态内联函数，将通道转换为字段
static inline unsigned int chan_to_field(unsigned int chan,
					 struct fb_bitfield *bf)
{
	// 将通道值与0xffff进行按位与操作，保留低16位
	chan &= 0xffff;
	// 将通道值右移16-bf->length位，得到高bf->length位
	chan >>= 16 - bf->length;
	// 将通道值左移bf->offset位，得到最终结果
	return chan << bf->offset;
}


// 设置颜色寄存器
static int mylcd_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
	unsigned int val;// 定义一个变量 val 用来存储颜色值

	/* dprintk("setcol: regno=%d, rgb=%d,%d,%d\n", regno, red, green, blue); */

	//  根据 regno 的值，判断是否在 0-15 之间，如果不在，则返回 1，表示错误
	switch (info->fix.visual) {
		case FB_VISUAL_TRUECOLOR:
			/* true-colour, use pseudo-palette */

			if (regno < 16) {
				u32 *pal = info->pseudo_palette;

				// 将红、绿、蓝三个通道的值转换为对应的字段值
				val  = chan_to_field(red,   &info->var.red);
				val |= chan_to_field(green, &info->var.green);
				val |= chan_to_field(blue,  &info->var.blue);

				// 将转换后的值存储到伪调色板中
				pal[regno] = val;
			}
			break;

		default:
			return 1;	/* unknown type */
	}

	return 0;
}




static struct fb_ops myfb_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= mylcd_setcolreg,// 设置颜色寄存器
	.fb_fillrect	= cfb_fillrect,// 填充矩形
	.fb_copyarea	= cfb_copyarea,// 复制区域
	.fb_imageblit	= cfb_imageblit,// 图像绘制
};






// ----------------------------------------

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
    if(uc_cmd == 0){
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
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    char* kern_buf;
    kern_buf = kmalloc(count, GFP_KERNEL);// 分配内存
    if (kern_buf == NULL) {
        return -ENOMEM; // 内存分配失败
    }
     copy_from_user(kern_buf, buf, count); // 从用户空间复制数据到内核空间

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
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
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




/* -----------------------------------------------------------------------*/


// 创建一个线程 用来 不断将 framebuffer 的数据  解析成 oled 要的数据， 其实就是将 
//  这个线程 一秒 钟 读取一次 framebuffer 的数据
//  oled_buf 里面有 1024个字节，每个字节有8位，每一位对应一个像素， 所以 像素会不断扫描这 1024 个字节，也就是扫描 1024* 8 个像素
static int oled_thread_func(void *param)
{

	unsigned char *p[8];
	unsigned char data[8];
	int i;
	int j;
	int line;
	int bit;
	unsigned char byte;
	unsigned char *fb  = myfb_info->screen_base;
	int k;	

	while (!kthread_should_stop()) // 判断是否需要停止线程
	{

		/* 1. 获取 framebuffer 的数据 */
		// 2. 将 framebuffer 的数据 转换格式
		k = 0;
		for (i = 0; i < 8; i++)
		{
			for (line = 0; line < 8; line++)
				p[line] = &fb[i*128 + line * 16];
			
			for (j = 0; j < 16; j++)
			{
				for (line = 0; line < 8; line++)
				{
					data[line] = *p[line];
					p[line] += 1;
				}

				for (bit = 0; bit < 8; bit++)
				{
					byte =  (((data[0]>>bit) & 1) << 0) |
							(((data[1]>>bit) & 1) << 1) |
							(((data[2]>>bit) & 1) << 2) |
							(((data[3]>>bit) & 1) << 3) |
							(((data[4]>>bit) & 1) << 4) |
							(((data[5]>>bit) & 1) << 5) |
							(((data[6]>>bit) & 1) << 6) |
							(((data[7]>>bit) & 1) << 7);

					oled_buf[k++] = byte;
				}
				
			}
		}
		

		/* 3. 通过SPI发送给OLED */
		// 通过 spi 发送到 oled 上
		for (i = 0; i < 8; i++)
		{
			OLED_DIsp_Set_Pos(0, i);
			oled_set_dc_pin(1);
			spi_write_datas(&oled_buf[i*128], 128);
		}
		






		/* 4. 休眠一会 ； HZ 休眠 1 秒 */
		schedule_timeout_interruptible(HZ);// HZ 休眠 1 秒
	}
	// 返回0
	return 0;
}




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



	/* A  分配 fb_info */
	dma_addr_t phy_addr;// 分配内存时需要的物理地址

	myfb_info = framebuffer_alloc(0, NULL);

	// B 设置 fb_info
	// B 1 LCD 分辨率 颜色  格式
	myfb_info->var.xres_virtual = myfb_info->var.xres = 128;
	myfb_info->var.yres_virtual = myfb_info->var.yres = 64;
	
	// 假设是 1 位颜色深度
	int bits_per_pixel = 1; // 假设是 1 位颜色深度，当前是 oled 所以是 1 位只有 黑白
	myfb_info->var.bits_per_pixel = bits_per_pixel;  /* rgb565 */
	// 这部分是 控制 颜色 的， 可以根据 颜色 深度 进行设置
	// if (bits_per_pixel == 16)
	// {
	// 	myfb_info->var.red.offset = 11;
	// 	myfb_info->var.red.length = 5;

	// 	myfb_info->var.green.offset = 5;
	// 	myfb_info->var.green.length = 6;

	// 	myfb_info->var.blue.offset = 0;
	// 	myfb_info->var.blue.length = 5;
	// }
	// else if (bits_per_pixel == 24 || bits_per_pixel == 32)
	// {
	// 	myfb_info->var.red.offset = 16;
	// 	myfb_info->var.red.length = 8;

	// 	myfb_info->var.green.offset = 8;
	// 	myfb_info->var.green.length = 8;

	// 	myfb_info->var.blue.offset = 0;
	// 	myfb_info->var.blue.length = 8;
	// }
	// else
	// {
	// 	return -EINVAL;
	// }


	

	/* B2  fix */
	strcpy(myfb_info->fix.id, "spi_dev_oled_lcd");
	myfb_info->fix.smem_len = myfb_info->var.xres * myfb_info->var.yres * myfb_info->var.bits_per_pixel / 8;

	/* fb的虚拟地址 */
	dma_set_coherent_mask(&spi->dev, DMA_BIT_MASK(32));// 是 32 位的地址空间； dma_set_coherent_mask是 设置 dma 的地址空间
	myfb_info->screen_base = dma_alloc_wc(&spi->dev, myfb_info->fix.smem_len, &phy_addr,  GFP_KERNEL);// 分配内存
	myfb_info->fix.smem_start = phy_addr;  /* fb的物理地址 */
	
	myfb_info->fix.type = FB_TYPE_PACKED_PIXELS;// 设置 fb 的类型
	myfb_info->fix.visual = FB_VISUAL_MONO10;// 这里是 单色的 OLED 所以是 FB_VISUAL_MONO10 ； 0 和 1

	myfb_info->fix.line_length = myfb_info->var.xres * myfb_info->var.bits_per_pixel / 8;// 计算每行的字节数
	// if (myfb_info->var.bits_per_pixel == 24)
	// 	myfb_info->fix.line_length = myfb_info->var.xres * 4;
	

	/* c. fbops */
	myfb_info->fbops = &myfb_ops;// 设置 fbops 目的 是 设置 操作函数
	myfb_info->pseudo_palette = pseudo_palette;// 伪调色板


	/* D 注册fb_info */
	register_framebuffer(myfb_info);



	// E  创建 内核线程， 用于处理 数据

	oled_buf = kmalloc(1024, GFP_KERNEL); // 分配 1024 字节的缓冲区

	//   初始化 OLED
	dc_pin_init(); // 初始化 DC 引脚
	oled_init(); // 初始化 OLED


	oled_thread = kthread_run(oled_thread_func, NULL, "oled_thread");//  创建内核线程

	return 0;
}

static void spidev_remove(struct spi_device *spi)
{

	kthread_stop(oled_thread); // 停止内核线程
	kfree(oled_buf); // 释放缓冲区


	// struct spidev_data	*spidev = spi_get_drvdata(spi);

    /* 2 释放 spi dev */
    gpiod_put(dc_gpio); // 释放 dc 引脚

	/* 1 注销字符设备 */
    device_destroy(spidev_class, MKDEV(major, 0));
    class_destroy(spidev_class);
	unregister_chrdev(major, "spi_dev_oled");

    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

}

static struct spi_driver spidev_spi_driver = {
	.driver = {
		.name =		"spidev,spi_dev_oled",
		.of_match_table = of_match_ptr(spidev_dt_ids),
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


