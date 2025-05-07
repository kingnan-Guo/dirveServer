2025/05/05 00:10
# 编写虚拟的gpio控制器
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义



设备树 

gpio_virtual: virtual_gpiocontroller {
    compatible = "kingnan,virtual_gpio";
    status = "okay";
    #gpio-cells = <2>;
    gpio-controller;
    #address-cells = <1>;
    #size-cells = <0>;
    ranges;
    interrupt-parent = <&gpio>;
    interrupts = <0 0>;
    reg = <0x0 0x0 0x0 0x0>;
    reg-names = "gpio";
    clocks = <&clk 0>;
    clock-names = "clk";
    reset-gpios = <&gpio 0 0>;
    reset-names = "reset";
}


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



root@raspberrypi:/home/kingnan/TEMP/gpio system# insmod gpio_system_dirver.ko 
root@raspberrypi:/home/kingnan/TEMP/gpio system# dmesg | tail

[   83.433042] gpio_system_dirver: loading out-of-tree module taints kernel.
[   83.433629] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_driver.c virtual_gpio_init 116
[   83.433858] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_driver.c virtual_gpio_probe 56
root@raspberrypi:/home/kingnan/TEMP/gpio system# cd /sys/class/gpio
root@raspberrypi:/sys/class/gpio# ls
export  gpiochip512  gpiochip566  gpiochip574  unexport
root@raspberrypi:/sys/class/gpio# cd gpiochip574
root@raspberrypi:/sys/class/gpio/gpiochip574# ls
base  device  label  ngpio  power  subsystem  uevent
root@raspberrypi:/sys/class/gpio/gpiochip574# cat label 
virtual_gpiocontroller
root@raspberrypi:/sys/class/gpio/gpiochip574# cat base 
574
root@raspberrypi:/sys/class/gpio/gpiochip574# cat ngpio 
4
root@raspberrypi:/sys/class/gpio/gpiochip574# 