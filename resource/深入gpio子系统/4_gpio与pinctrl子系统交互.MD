2025/05/05 00:10
# 编写虚拟的gpio控制器
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义



设备树 
    /** pinctroller */
	virtual_pincontroller {
		compatible = "kingnan,virtual_controller_driver";

        led_pin: led_pin {
			functions = "gpio";/** pin0 复用为 gpio 功能 */
			groups = "pin0";
			configs = <0x33>;
        }
		i2cgrp: i2cgrp {
			functions = "i2c", "i2c";
			groups = "pin0", "pin1";
			configs = <0x11 0x22>;/** 配置值, 会在 virtual_pinconf_get 中获得到 配置值 */
		};
	};


	/** 虚拟的 gpio 节点 */

	gpio_virtual: virtual_gpiocontroller {
		compatible = "kingnan,virtual_gpio";
		gpio-controller;
		#gpio-cells = <2>;/**  两个整数描述 gpio */
		ngpios = <4>;/**  4个gpio */
	};

	gpio_virtual_led{
		compatible = "kingnan,gpio_virtual_led";
		gpio = <&gpio_virtual 0 0>;/** 使用 虚拟的 gpio 的 第 0 个引脚， 输出 模式 */
        pinctrl-names = "default";
        pinctrl-0 = <&led_pin>;/** 使用 virtual_pincontroller 节点的  led_pin */
	};





# 流程


# 执行顺序


# 内部机制


# Makefile

# # gpio_system gpio 子系统 ---------------------
gpio_system_dirver-y := $(MODULES_DIR)/gpio_system/virtual_gpio_driver.o
obj-m += gpio_system_dirver.o
gpio_system_client-y := $(MODULES_DIR)/gpio_system/virtual_gpio_client.o
obj-m += gpio_system_client.o



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

make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 

# 扩展



gpio0{
    gpio-ranges = <&pinctrl 0 0 4>;/** 表明 gpio0 的第 0 个引脚 映射为 pinctrl 子系统中的 第 0 个 ， 映射 4个引脚 */
}

gpio1{
    gpio-ranges = <&pinctrl 0 4 4>;/** 表明 gpio1 的第 0 个引脚 映射为 pinctrl 子系统中的 第 4 个 ， 映射 4个引脚 */
}


所以说
gpio1 中可能有 4 个引脚，但是在整个系统中 对应的 可能 是 4、5、6、7
在映射过程中 使用了 gpio-ranges 来进行映射， struct gpio_pin_range 中 的 struct pinctrl_gpio_range 中的 pin_base ：  pinctrl 设备中的引脚编号，base gpio系统中的引脚编号


程序调用过程是 
1、程序调用 gpiod_get ，会导致 struct gpio_device 中的 gpio_chip 中的 request 函数被调用
2、request 函数 会把 gpio number 转换为 pinctrl 子系统中的引脚编号，
3、然后调用 pinctrl 子系统中的 pmxops 引脚复用函数 调用  request 或者 gpio_set_direction 函数，进行引脚的配置






