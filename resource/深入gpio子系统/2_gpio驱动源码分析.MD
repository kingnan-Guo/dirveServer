2025/05/05 00:10
# gpio驱动源码分析
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义

在 反汇编的设备树中  [反汇编设备树文件](从fdt反汇编而来的设备树.dts) 查找 gpio-controller 
查找 父级 找到 soc


查找 gpio@7e200000
root@e99a2e222cc0:/opt/sources/linux-rpi-6.6.y# grep "gpio@7e200000" * -nr


在 
sources/linux-rpi-6.6.y/arch/arm64/boot/dts/broadcom/.bcm2710-rpi-3-b-plus.dtb.dts.tmp
中 有 gpio: gpio@7e200000




gpio-ranges = <&gpio 0 0 54>; 是用来 联系 pinctrl 子系统 和 gpio 子系统的
使用  of_parse_phandle_with_fixed_args() 函数 来解析 gpio-ranges 属性, 0 0 54 是 args0 args1 args2
args1： 属于哪个控制器

```C
static inline int of_parse_phandle_with_fixed_args(const struct device_node *np,
						   const char *list_name,
						   int cell_count,
						   int index,
						   struct of_phandle_args *out_args)

```


gpio_chip 中 的 base 值 是由 of_parse_phandle_with_fixed_args() 函数 来获取的


拼装完成 gpio_chip  后 ，去注册 gpio_chip ， 驱动程序 通过 gpiochip_add_data(struct gpio_chip *chip, void *data) 向上 注册到 gpiolib 中， 其他就可以使用统一的 接口 访问 到 驱动程序了


在  [sources/linux-rpi-6.6.y/drivers/gpio/gpiolib.c] 的 gpiochip_add_data_with_key()  函数中 创建 gpio_deivces ，设置 gpio_chip 指向 	gdev->chip = gc;


最终  在 gpiochip_add_data_with_key 中 调用  gpiodev_add_to_list () 将 gdev 添加到 gpiodev_list 中
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

