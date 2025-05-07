2025/05/07 22:33
# 4_gpio与pinctrl子系统交互
    分支
        dirver_raspberry_gpio_connect_pinctrl_v0.2.7.1

    文件
        ./modules/app_gpio_system/app_gpio_system.c

		./dirverModules/gpio_system/virtual_gpio_client.c
		./dirverModules/gpio_system/virtual_gpio_driver.c
		./dirverModules/pinctrl_system/virtual_controller_driver.c


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
		led-gpios = <&gpio_virtual 0 0>;/** 使用 虚拟的 gpio 的 第 0 个引脚， 输出 模式 */
        pinctrl-names = "default";
        pinctrl-0 = <&led_pin>;/** 使用 virtual_pincontroller 节点的  led_pin */
	};





# 流程


# 执行顺序


# 内部机制


# Makefile

# 同时打包 两个子系统 还有  main.c

# # pinctrl_system  pinctrl 子系统 ---------------------		
pinctrl_system_dirver-y := $(MODULES_DIR)/pinctrl_system/virtual_controller_driver.o
obj-m += pinctrl_system_dirver.o
pinctrl_system_clinet-y := $(MODULES_DIR)/pinctrl_system/virtual_client_driver.o
obj-m += pinctrl_system_clinet.o

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








root@raspberrypi:/home/kingnan/TEMP/pinctrl 子系统/printctrl 实现# insmod pinctrl_system_dirver.ko 
root@raspberrypi:/home/kingnan/TEMP/gpio system# insmod gpio_system_dirver.ko 
root@raspberrypi:/home/kingnan/TEMP/gpio system# insmod gpio_system_client.ko 
root@raspberrypi:/home/kingnan/TEMP/gpio system# dmesg | tail
[   97.428180] pinctrl_system_dirver: loading out-of-tree module taints kernel.
[   97.429520] virtual_controller_driver virtual_pincontroller: virtual_controller_driver_probe
[   97.429647] virtual_controller_driver virtual_pincontroller: Registered pinctrl device
[   97.429672] virtual_controller_driver virtual_pincontroller: Failed to create debugfs dir: -13, continuing without debugfs
[  111.728867] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_driver.c virtual_gpio_init 116
[  111.729191] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_driver.c virtual_gpio_probe 56
[  115.537420] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_init line 138
[  115.537833] set pin0 as gpio
[  115.537847] config pin0 as 0x33
[  115.537906] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_probe line 85
root@raspberrypi:/home/kingnan/TEMP/gpio system# ./main /dev/virtual_gpio_led_0 on
root@raspberrypi:/home/kingnan/TEMP/gpio system# dmesg | tail
[  115.537906] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_probe line 85
[  120.728624] hwmon hwmon1: Voltage normalised
[  122.745060] hwmon hwmon1: Undervoltage detected!
[  128.793945] hwmon hwmon1: Voltage normalised
[  130.810182] hwmon hwmon1: Undervoltage detected!
[  136.523662] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_open line 31
[  136.523693] set pin 0 as output low
[  136.523705] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_write line 50
[  136.523712] set pin 0 as high
[  136.523719] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_close line 68
root@raspberrypi:/home/kingnan/TEMP/gpio system# ./main /dev/virtual_gpio_led_0 off
root@raspberrypi:/home/kingnan/TEMP/gpio system# dmesg | tail
[  136.523662] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_open line 31
[  136.523693] set pin 0 as output low
[  136.523705] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_write line 50
[  136.523712] set pin 0 as high
[  136.523719] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_close line 68
[  141.508206] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_open line 31
[  141.508251] set pin 0 as output low
[  141.508275] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_write line 50
[  141.508296] set pin 0 as low
[  141.508313] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_close line 68
root@raspberrypi:/home/kingnan/TEMP/gpio system# 









