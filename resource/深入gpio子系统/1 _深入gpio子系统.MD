2025/05/05 00:10
# 深入gpio子系统
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义

gpiod_set_value 函数在 sources/linux-rpi-6.6.y/drivers/gpio/gpiolib.c 中

gpiolib 是 中间层
gpiolib向上 提供统一的接口 
gpiolib 向下 管理 多个驱动程序

驱动程序 通过 gpiochip_add_data(struct gpio_chip *chip, void *data) 向上 注册到 gpiolib 中， 其他就可以使用统一的 接口 访问 到 驱动程序了


对于 一个 gpio controller 必定会使用一个 结构体俩表示，这个结构体就是 gpio_chip， 先构建出 struct gpio_chip (sources/linux-rpi-6.6.y/include/linux/gpio/driver.h) 结构体 ，然后 使用 gpiochip_add_data 来注册，就会 创建 struct gpio_device (sources/linux-rpi-6.6.y/drivers/gpio/gpiolib.h) 结构体
    GPIO引进脚信息： 输出还是输入引脚， 要配置方向， 要配置电平， 要配置中断， 要配置中断触发方式
    控制引脚的函数
    中断相关的函数： 可以触发中断




```c

struct gpio_device {
	struct device		dev;
	struct cdev		chrdev;
	int			id;
	struct device		*mockdev;
	struct module		*owner;
	struct gpio_chip	*chip;// 包含各类操作函数
	struct gpio_desc	*descs;// 数组 ， 用来描述引脚，引脚的编号，引脚的名称，引脚的配置信息，每一个引脚对应一个 gpio_desc 结构体， 每一个引脚会 对应数组里的一项 
	int			base;
	u16			ngpio;
	const char		*label;
	void			*data;
	struct list_head        list;
	struct blocking_notifier_head line_state_notifier;
	struct blocking_notifier_head device_notifier;
	struct rw_semaphore	sem;
}




struct gpio_desc {
	struct gpio_device	*gdev;// 属于 哪个 GPIO controller， 根据 gpio 控制器 找到 对应 的驱动程序 构造的 gpio_chip， gpio_chip 里面的这些函数 来操作硬件
	unsigned long		flags;// 标记位
	const char		*label;
    const char		*name;
    struct device_node	*hog;
};

```
struct gpio_device





假设 一个芯片里面 有 两个 gpio controller （gpio controller 0、 gpio controller 1） ， 所有的  gpio controller 会放在 一个 链表里面

gpio controller  0 控制 4 个 引脚，对于 gpio controller 0 中 构造 了一个 struct gpio_device ， 

    gpio controller 0 的  struct gpio_device 中 的 base 是 0， ngpio 是 4， 使用 编码 操作这个引脚的时候   是 0、1、2、3



```C

struct gpio_device {
	struct device		dev;
	struct cdev		chrdev;
	int			id;
	struct device		*mockdev;
	struct module		*owner;
	struct gpio_chip	*chip;// 包含各类操作函数
	struct gpio_desc	*descs;// 数组 ， 用来描述引脚，引脚的编号，引脚的名称，引脚的配置信息，每一个引脚对应一个 gpio_desc 结构体， 每一个引脚会 对应数组里的一项 ；descs 中 有 4 个 gpio_desc 结构体 [0、1、2、3]

	int			base;   // 0
	u16			ngpio;   // 4
	const char		*label;
	void			*data;
	struct list_head        list;
	struct blocking_notifier_head line_state_notifier;
	struct blocking_notifier_head device_notifier;
	struct rw_semaphore	sem;
}

设备树

led {
	compatible = "brcm,bcm2835-gpio";
    led-gpios = <&gpio1 10 0>;// 使用到了 第一个 gpio controller 1 的 第 10 个引脚，但是当前 rpi 3B+ 只有一个 引脚控制器
};


在 自己的驱动程序 中 使用 gpio_desc *_gpio = gpiod_get(&pdev-dev, "led", 0); 获取到 第一个 gpio controller 1 的 第 10 个引脚 的 _gpio ，

根据 _gpio 的是 gpio_desc 指针，根据 gpio1 找到 gpio controller 1 （控制器 1 ），然后 根据 gpio controller 1 找到 gpio_chip ，然后 根据 gpio_chip 找到 gpio_device ，然后 根据 gpio_device 找到 gpio_desc 数组，然后 根据 gpio_desc 数组 找到 第 10 个 gpio_desc 结构体，然后 根据 第 10 个 gpio_desc 结构体 找到 第 10 个引脚


然后使用 gpiod_set_value(_gpio, 1);  设置引脚电平为 1， 找到 gpio_chip 调用 内部的 set, 要传入 offset 偏移量，根据 gpio_desc 找到 offset 偏移量，然后调用 gpio_chip 中的 set 函数，然后调用 gpio controller 1 中的 set 函数，然后设置引脚电平为 1；

offset 是 使用 当前 获得的 _gpio  - gpio_device->descs 得到的， gpio_device->descs 是 gpio_desc 数组， gpio_device->descs + offset 就是 第 offset 个 gpio_desc 结构体，然后 根据 第 offset 个 gpio_desc 结构体 找到 第 offset 个引脚，然后设置引脚电平为 1；

static inline int gpio_chip_hwgpio(const struct gpio_desc *desc)// 获得 offset 偏移量
{
	return desc - &desc->gdev->descs[0];
}



struct gpio_chip {
	void			(*set)(struct gpio_chip *gc, unsigned int offset, int value);
}

```


gpio controller  1 控制 32 个 引脚，
    gpio controller 1 的  struct gpio_device 中 的 base 是 4， ngpio 是 32， 使用 编码 操作这个引脚的时候   是 4、5、... 、36








# 流程


# 执行顺序


# 内部机制


# Makefile
# # XXXX ---------------------
XXXX-y := $(MODULES_DIR)/XXXX/XXXX.o
obj-m := XXXX.o


# 执行命令


insmod
rmmod

chmod +x main

ps -ef | grep main
kill -9 PID

ls /proc/device-tree/
ls /sys/devices/platform/
dmesg | tail
cat /proc/devices  
cd /sys/class 



# 扩展

