2025/05/07 21:21
# 编写虚拟的gpio控制器
    分支
        dirver_raspberry_app_gpio_system_v0.2.7

    文件
        ./modules/app_gpio_system/app_gpio_system.c
        ./dirverModules/gpio_system/virtual_gpio_client.c
        ./dirverModules/gpio_system/virtual_gpio_driver.c

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




root@raspberrypi:/home/kingnan/TEMP/gpio system# cd /sys/class/gpio
root@raspberrypi:/sys/class/gpio# ls
export  gpiochip512  gpiochip566  gpiochip574  unexport
root@raspberrypi:/sys/class/gpio# cd gpiochip574
root@raspberrypi:/sys/class/gpio/gpiochip574# ls
base  device  label  ngpio  power  subsystem  uevent
root@raspberrypi:/sys/class/gpio/gpiochip574# cat label 
virtual_gpiocontroller
root@raspberrypi:/sys/class/gpio/gpiochip574# echo 575 > export
bash: export: Permission denied
root@raspberrypi:/sys/class/gpio/gpiochip574# cd ../
root@raspberrypi:/sys/class/gpio# ls
export  gpiochip512  gpiochip566  gpiochip574  unexport
root@raspberrypi:/sys/class/gpio# echo 575 > export
root@raspberrypi:/sys/class/gpio# ls
export  gpio575  gpiochip512  gpiochip566  gpiochip574  unexport
root@raspberrypi:/sys/class/gpio# cd gpio575
root@raspberrypi:/sys/class/gpio/gpio575# ls
active_low  device  direction  power  subsystem  uevent  value
root@raspberrypi:/sys/class/gpio/gpio575# cat value 
0
root@raspberrypi:/sys/class/gpio/gpio575# echo in > direction 
root@raspberrypi:/sys/class/gpio/gpio575# cat value 
0
root@raspberrypi:/sys/class/gpio/gpio575# dmesg | tail
[30778.292369] hwmon hwmon1: Undervoltage detected!
[30788.083597] get pin 1, it's val = 0
[30788.083868] get pin 1, it's val = 0
[30802.958602] set pin 1 as input
[30808.280159] get pin 1, it's val = 0
[30808.280266] get pin 1, it's val = 0
[30810.548142] hwmon hwmon1: Voltage normalised
[30814.580088] hwmon hwmon1: Undervoltage detected!
[30826.675954] hwmon hwmon1: Voltage normalised
[30828.692014] hwmon hwmon1: Undervoltage detected!
root@raspberrypi:/sys/class/gpio/gpio575# echo out > direction 
root@raspberrypi:/sys/class/gpio/gpio575# dmesg | tail
[30828.692014] hwmon hwmon1: Undervoltage detected!
[30834.739913] hwmon hwmon1: Voltage normalised
[30836.756108] hwmon hwmon1: Undervoltage detected!
[30844.820812] hwmon hwmon1: Voltage normalised
[30846.835920] hwmon hwmon1: Undervoltage detected!
[30850.867889] hwmon hwmon1: Voltage normalised
[30852.883878] hwmon hwmon1: Undervoltage detected!
[30858.931872] hwmon hwmon1: Voltage normalised
[30859.501593] set pin 1 as output low
[30860.948703] hwmon hwmon1: Undervoltage detected!

root@raspberrypi:/sys/class/gpio/gpio575# echo 1 > value 
root@raspberrypi:/sys/class/gpio/gpio575# cat value 
1
root@raspberrypi:/sys/class/gpio/gpio575# dmesg | tail
[30883.123708] hwmon hwmon1: Undervoltage detected!
[30891.187641] hwmon hwmon1: Voltage normalised
[30893.203631] hwmon hwmon1: Undervoltage detected!
[30900.345010] set pin 1 as high
[30903.685379] get pin 1, it's val = 1
[30903.685461] get pin 1, it's val = 1
[30909.332614] hwmon hwmon1: Voltage normalised
[30911.347545] hwmon hwmon1: Undervoltage detected!
[30919.411489] hwmon hwmon1: Voltage normalised
[30921.427485] hwmon hwmon1: Undervoltage detected!





root@raspberrypi:/home/kingnan/TEMP/gpio system# insmod gpio_system_client.ko 
root@raspberrypi:/home/kingnan/TEMP/gpio system# lsmod 
Module                  Size  Used by
gpio_system_client     12288  0
gpio_system_dirver     12288  1


root@raspberrypi:/home/kingnan/TEMP/gpio system# ./main /dev/virtual_gpio_led_0 off
root@raspberrypi:/home/kingnan/TEMP/gpio system# dmesg | tail
[30502.102085] hwmon hwmon1: Undervoltage detected!
[30520.245910] hwmon hwmon1: Voltage normalised
[30522.261938] hwmon hwmon1: Undervoltage detected!
[30523.674398] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_open line 31
[30523.674421] set pin 0 as output low
[30523.674471] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_write line 50
[30523.674478] set pin 0 as low
[30523.674486] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_close line 68
[30530.329896] hwmon hwmon1: Voltage normalised
[30532.341868] hwmon hwmon1: Undervoltage detected!
root@raspberrypi:/home/kingnan/TEMP/gpio system# ./main /dev/virtual_gpio_led_0 on
root@raspberrypi:/home/kingnan/TEMP/gpio system# dmesg | tail
[30532.341868] hwmon hwmon1: Undervoltage detected!
[30546.453760] hwmon hwmon1: Voltage normalised
[30548.469751] hwmon hwmon1: Undervoltage detected!
[30554.517699] hwmon hwmon1: Voltage normalised
[30556.533699] hwmon hwmon1: Undervoltage detected!
[30558.702158] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_open line 31
[30558.702197] set pin 0 as output low
[30558.702218] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_write line 50
[30558.702232] set pin 0 as high
[30558.702248] /opt/github/dirveServer/dirverModules/gpio_system/virtual_gpio_client.c virtual_gpio_client_close line 68
root@raspberrypi:/home/kingnan/TEMP/gpio system# cat /sys/class/gpio
cat: /sys/class/gpio: Is a directory