2025/05/08 00:10
# 编写虚拟的gpio控制器
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_gpio_system/app_gpio_system.c
        ./dirverModules/gpio_pinctrl_system/virtual_controller_driver.c
		./dirverModules/gpio_pinctrl_system/virtual_gpio_client.c
		./dirverModules/gpio_pinctrl_system/virtual_gpio_driver.c


# 定义



设备树 

	这里可以不指定 led_pin ，在代码中使用  rang 相关的函数，让 pinctrl 子系统自动配置引脚


    /** pinctroller */
	pinctrl_virtual:virtual_pincontroller {
		compatible = "kingnan,virtual_controller_driver";
		i2cgrp: i2cgrp {
			functions = "i2c", "i2c";
			groups = "pin0", "pin1";
			configs = <0x11 0x22>;/** 配置值, 会在 virtual_pinconf_get 中获得到 配置值 */
		};
	};

	virtual_i2c {
		compatible = "kingnan,virtual_i2c";
		pinctrl-names = "default"; /* 修正为 pinctrl-names */
		pinctrl-0 = <&i2cgrp>;
	};



	/** 虚拟的 gpio 节点 */

	gpio_virtual: virtual_gpiocontroller {
		compatible = "kingnan,virtual_gpio";
		gpio-controller;
		#gpio-cells = <2>;/**  两个整数描述 gpio */
		ngpios = <4>;/**  4个gpio */
        gpio-ranges = <&pinctrl_virtual 0 0 4>;/**  gpio 子系统 与 pinctrl 子系统 有联系， gpio 的 第0 号 引脚 映射到 pinctrl_virtual 的 0 号引脚 映射 4 个*/
	};

	gpio_virtual_led{
		compatible = "kingnan,gpio_virtual_led";
		led-gpios = <&gpio_virtual 0 0>;/** 使用 虚拟的 gpio 的 第 0 个引脚， 输出 模式 */
        /** pinctrl-names = "default"; 
         pinctrl-0 = <&led_pin>;  去掉这部分，在代码中使用  rang 相关的函数，让 pinctrl 子系统自动配置引脚 */
	};












# 流程


 在 gpio: virtual_gpio_driver.c 程序中 提供 gpio_chip -> request
 
 在 pinctrl : virtual_controller_driver.c程序中 提供 pmxops -> request 或者 pmxops->gpio_request_enable






# 执行顺序


# 内部机制


# Makefile

# 同时打包 三个驱动 还有  main.c
# # gpio_system pinctrl_system   子系统 ---------------------	
pinctrl_system_dirver-y := $(MODULES_DIR)/gpio_pinctrl_system/virtual_controller_driver.o
obj-m += pinctrl_system_dirver.o
gpio_system_dirver-y := $(MODULES_DIR)/gpio_pinctrl_system/virtual_gpio_driver.o
obj-m += gpio_system_dirver.o
gpio_system_client-y := $(MODULES_DIR)/gpio_pinctrl_system/virtual_gpio_client.o
obj-m += gpio_system_client.o


设备树 ：[gpio子系统与pinctrl子系统之间的联系](bcm2710-rpi-3-b-plus.dts_gpio子系统与pinctrl子系统之间的联系)




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

echo "7 4 1 7" > /proc/sys/kernel/printk

make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 


cp bcm2710-rpi-3-b-plus.dtb /boot/firmware/ -r
# 扩展





root@raspberrypi:/home/kingnan/TEMP/gpio system/gpio_pinctrl_system联系# ls
bcm2710-rpi-3-b-plus.dtb  gpio_system_client.ko  gpio_system_dirver.ko  main  pinctrl_system_dirver.ko
root@raspberrypi:/home/kingnan/TEMP/gpio system/gpio_pinctrl_system联系# chmod +x main
root@raspberrypi:/home/kingnan/TEMP/gpio system/gpio_pinctrl_system联系# insmod pinctrl_system_dirver.ko 
root@raspberrypi:/home/kingnan/TEMP/gpio system/gpio_pinctrl_system联系# insmod gpio_system_dirver.ko 
root@raspberrypi:/home/kingnan/TEMP/gpio system/gpio_pinctrl_system联系# insmod gpio_system_client.ko 
root@raspberrypi:/home/kingnan/TEMP/gpio system/gpio_pinctrl_system联系# dmesg | tail
[  241.861956] hwmon hwmon1: Undervoltage detected!
[  251.324048] pinctrl_system_dirver: loading out-of-tree module taints kernel.
[  251.326016] virtual_controller_driver virtual_pincontroller: virtual_controller_driver_probe
[  251.326190] virtual_controller_driver virtual_pincontroller: Registered pinctrl device
[  251.326213] virtual_controller_driver virtual_pincontroller: Failed to create debugfs dir: -13, continuing without debugfs
[  259.696109] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_driver.c virtual_gpio_init 120
[  259.696464] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_driver.c virtual_gpio_probe 56
[  266.063109] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_client.c virtual_gpio_client_init line 137
[  266.063457] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_client.c virtual_gpio_client_probe line 85
[  266.063526] set and enable pin pin0 as GPIO
root@raspberrypi:/home/kingnan/TEMP/gpio system/gpio_pinctrl_system联系# ./main /dev/virtual_gpio_led_0 on
root@raspberrypi:/home/kingnan/TEMP/gpio system/gpio_pinctrl_system联系# ./main /dev/virtual_gpio_led_0 off
root@raspberrypi:/home/kingnan/TEMP/gpio system/gpio_pinctrl_system联系# dmesg | tail -n 30
[  251.324048] pinctrl_system_dirver: loading out-of-tree module taints kernel.
[  251.326016] virtual_controller_driver virtual_pincontroller: virtual_controller_driver_probe
[  251.326190] virtual_controller_driver virtual_pincontroller: Registered pinctrl device
[  251.326213] virtual_controller_driver virtual_pincontroller: Failed to create debugfs dir: -13, continuing without debugfs
[  259.696109] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_driver.c virtual_gpio_init 120
[  259.696464] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_driver.c virtual_gpio_probe 56
[  266.063109] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_client.c virtual_gpio_client_init line 137
[  266.063457] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_client.c virtual_gpio_client_probe line 85
[  266.063526] set and enable pin pin0 as GPIO
[  308.756656] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_client.c virtual_gpio_client_open line 31
[  308.756761] set pin 0 as output low
[  308.756797] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_client.c virtual_gpio_client_write line 50
[  308.756814] set pin 0 as high
[  308.756834] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_client.c virtual_gpio_client_close line 68
[  311.919814] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_client.c virtual_gpio_client_open line 31
[  311.919863] set pin 0 as output low
[  311.919889] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_client.c virtual_gpio_client_write line 50
[  311.919911] set pin 0 as low
[  311.919928] /opt/github/dirveServer/dirverModules/gpio_pinctrl_system/virtual_gpio_client.c virtual_gpio_client_close line 68

不一样的是 configs 没有设置值 ，因为 没有设置引脚的电气特性，所以没有设置值

root@raspberrypi:/home/kingnan/TEMP/gpio system/gpio_pinctrl_system联系#  cd /sys/kernel/debug/pinctrl/
root@raspberrypi:/sys/kernel/debug/pinctrl# ls
3f200000.gpio-pinctrl-bcm2835  pinctrl-devices  pinctrl-handles  pinctrl-maps  virtual_pincontroller
root@raspberrypi:/sys/kernel/debug/pinctrl# cd virtual_pincontroller
root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# cat pinmux-pins
Pinmux settings per pin
Format: pin (name): mux_owner gpio_owner hog?
pin 0 (pin0): (MUX UNCLAIMED) virtual_gpiocontroller:574
pin 1 (pin1): (MUX UNCLAIMED) (GPIO UNCLAIMED)
pin 2 (pin2): (MUX UNCLAIMED) (GPIO UNCLAIMED)
pin 3 (pin3): (MUX UNCLAIMED) (GPIO UNCLAIMED)
root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# cat pinconf-pins 
Pin config settings per pin
Format: pin (name): configs
pin 0 (pin0): 0x0
pin 1 (pin1): 0x0
pin 2 (pin2): 0x0
pin 3 (pin3): 0x0








# gpio 子系统 的  sysfs 接口

echo 575 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio575/direction
echo 1 > /sys/class/gpio/gpio575/value
echo 0 > /sys/class/gpio/gpio575/value
echo 575 > /sys/class/gpio/unexport


echo 575 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio575/direction
cat /sys/class/gpio/gpio575/value
echo 575 > /sys/class/gpio/unexport






# pinctrl 子系统 的 sysfs 接口

echo virtual_pincontroller > /sys/kernel/debug/pinctrl/pinctrl-devices
echo pin0 > /sys/kernel/debug/pinctrl/pinctrl-handles
echo virtual_gpiocontroller > /sys/kernel/debug/pinctrl/pinctrl-maps
echo 575 > /sys/kernel/debug/pinctrl/pinctrl-handles
echo virtual_gpiocontroller > /sys/kernel/debug/pinctrl/pinctrl-maps
echo 575 > /sys/kernel/debug/pinctrl/pinctrl-handles







# sysfs 接口 命令 使用


root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# cd /sys/bus/gpio/devices/
root@raspberrypi:/sys/bus/gpio/devices# ls
gpiochip0  gpiochip1  gpiochip2
root@raspberrypi:/sys/bus/gpio/devices# cd /sys/class/gpio
root@raspberrypi:/sys/class/gpio# ls
export  gpiochip512  gpiochip566  gpiochip574  unexport
root@raspberrypi:/sys/class/gpio# cd gpiochip574
root@raspberrypi:/sys/class/gpio/gpiochip574# ls
base  device  label  ngpio  power  subsystem  uevent
root@raspberrypi:/sys/class/gpio/gpiochip574# cat label 
virtual_gpiocontroller
root@raspberrypi:/sys/class/gpio/gpiochip574# cd ../
root@raspberrypi:/sys/class/gpio# echo 575 > /sys/class/gpio/export 
root@raspberrypi:/sys/class/gpio# ls
export  gpio575  gpiochip512  gpiochip566  gpiochip574  unexport
root@raspberrypi:/sys/class/gpio# cd gpio575
root@raspberrypi:/sys/class/gpio/gpio575# ls
active_low  device  direction  power  subsystem  uevent  value
root@raspberrypi:/sys/class/gpio/gpio575# cd ../
root@raspberrypi:/sys/class/gpio# cat /sys/kernel/debug/gpio 
gpiochip0: GPIOs 512-565, parent: platform/3f200000.gpio, pinctrl-bcm2835:
 gpio-512 (ID_SDA              )
 gpio-513 (ID_SCL              )
 gpio-514 (GPIO2               )
 gpio-515 (GPIO3               )
 gpio-516 (GPIO4               )
 gpio-517 (GPIO5               )
 gpio-518 (GPIO6               )






# =============


GPIO 使用 pinctrl 设置 pin0 为 gpio 功能



我现在的理解 pinctr 子系统 是定义 有多个个 引脚 且 统一管理，
gpio子系统 与 pinctrl 子系统建立联系， 配置 gpio 复用 pinctrl 中的 一些功能，不知道对不对










# AI 总结


Pinctrl 子系统的作用：
	Pinctrl（Pin Control）子系统主要负责 引脚复用（Pin Multiplexing） 和 引脚配置（Pin Configuration）。
	现代 SoC（片上系统）中，一个物理引脚通常可以复用为多种功能（如 GPIO、I2C、SPI、UART 等）。Pinctrl 子系统统一管理这些引脚的 功能选择 和 电气特性（如上拉/下拉电阻、驱动强度、偏置等）。
	它通过设备树（Device Tree）或驱动代码定义引脚的“状态”（state），如“default”、“sleep”等，每个状态对应一组引脚配置。
GPIO 子系统与 Pinctrl 的关系：
	GPIO（General Purpose Input/Output）子系统专注于 引脚的输入输出控制，例如设置引脚为高/低电平、读取引脚状态等。
	如果一个引脚被用作 GPIO 功能，通常需要先通过 Pinctrl 子系统将该引脚 复用 为 GPIO 模式。例如，一个引脚可能默认配置为 I2C SDA，但你想用它作为 GPIO，则需要 Pinctrl 将其功能切换为 GPIO。
	在 Linux 内核中，GPIO 驱动通常依赖 Pinctrl 子系统来完成引脚的初始配置（复用和电气特性），然后 GPIO 子系统接管引脚的实际操作。

你的理解中的正确点：
	你提到“pinctrl 子系统定义多个引脚且统一管理”，这是对的。Pinctrl 确实是集中管理引脚的复用和配置。
	你提到“GPIO 子系统与 Pinctrl 子系统建立联系，配置 GPIO 复用 Pinctrl 中的一些功能”，这也是对的。GPIO 子系统需要 Pinctrl 来设置引脚为 GPIO 模式。


需要修改的是 ：
	你的描述中，“配置 GPIO 复用 Pinctrl 中的一些功能”这句话可能稍显模糊。实际上，GPIO 本身是 Pinctrl 管理的一种复用功能。Pinctrl 的职责是将引脚配置为 GPIO 或其他功能（如 SPI、I2C），而 GPIO 子系统则负责 GPIO 功能的具体操作（如设置输入/输出、读写电平）。

Pinctrl 和 GPIO 子系统的交互通常通过 设备树 或驱动代码实现。例如，在设备树中，你会看到类似以下的配置：

总结：
	Pinctrl 子系统负责引脚的 复用 和 配置，包括将引脚设置为 GPIO 模式。
	GPIO 子系统负责引脚在 GPIO 模式下的 具体操作（如输入/输出控制）。
	两者通过设备树或驱动代码协作，Pinctrl 先配置引脚功能，GPIO 再操作引脚。