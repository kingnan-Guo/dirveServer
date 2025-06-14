2025/06/11 00:10
# gpio模拟i2c
    分支
        dirver_raspberry_i2c_gpio_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c



[text](bcm2710-rpi-3-b-plus-i2c-gpio.dts)



# 定义




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




# 设备树
使用  驱动
sources/linux-rpi-6.6.y/drivers/i2c/busses/i2c-gpio.c

/{
    i2c_gpio: i2c_gpio {
        compatible = "i2c-gpio";/**  */
        #address-cells = <1>;/** 用来描述 下方 的 reg 的 需要一个参数描述 地址 ， 地址用一个 整数来表示*/
        #size-cells = <0>;/** 用来描述  reg 的第二个参数 size*/
        pinctrl-names = "default";
        pinctrl-0 = <&i2c_gpio_pins>;/** i2c_gpio_pins 是   */
        gpios = <&gpio 17 0>, <&gpio 18 0>;
        clock-frequency = <100000>;/** 指定频率*/
        ds1339: rtc@68 {
            compatible = "ds1339";
            reg = <0x68>;/** */
            status = "disabled";
            state = "okay";
        }
    }
}







/{
    i2c_gpio: i2c_gpio {
        compatible = "i2c-gpio";/**  */
        #address-cells = <1>;/** 用来描述 下方 的 reg 的 需要一个参数描述 地址 ， 地址用一个 整数来表示*/
        #size-cells = <0>;/** 用来描述  reg 的第二个参数 size*/
        pinctrl-names = "default";
        pinctrl-0 = <&i2c_gpio_pins>;/** i2c_gpio_pins 是 pinctrl  */
        gpios = <&gpio 5 0>, <&gpio 6 0>;/** GPIO5 是 SDA   GPIO6 是 SCL*/
        clock-frequency = <100000>;/** 指定频率*/
        at24c02@50 {
            compatible = "kingnan,i2c_dev";
            reg = <0x50>;
            pagesize = <8>;
            status = "okay";
        };
    };



    &gpio {
        i2c_gpio_pins: i2c_gpio_pins {
            compatible = "brcm,bcm2835-gpio";
            brcm,pins = <17 18>; /* GPIO17 和 GPIO18 */
            brcm,function = <0>; /* 0 表示 GPIO 功能（由 i2c-gpio 驅動控制） */
            brcm,pull = <2>; /* 2 表示上拉電阻，I2C 通常需要上拉 */
        };
    }

}


涉及到 i2c-gpio.c 和 设备树
会打包 linux 内核中的 i2c-gpio.c 为 i2c-gpio.ko， 然后修改设备树  指定 sda scl 具体指向 哪个 gpio

 命令：   make  modules -j4       打包 i2c-gpio.ko






如何使用 i2c-gpio


指定频率（两种方法）
    i2c-gpio,delay-us = <5>;/** 100KHz */
    clock-frequency = <100000>;/** 100KHz */






接下来 要 使用 一下 i2c-gpio





 make  modules -j$(nproc) 
编译完  找到
sources/linux-rpi-6.6.y/drivers/i2c/busses/i2c-gpio.ko


修改设备树

打包设备树
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 



# 接线

 SCL  GPIO6
 SDA  GPIO5

 然后 就可以正常使用了 ， 分配了 i2c-11 这个总线




root@raspberrypi:/home/kingnan/TEMP/i2c-gpio# ls
bcm2710-rpi-3-b-plus.dtb  i2c-gpio.ko
root@raspberrypi:/home/kingnan/TEMP/i2c-gpio# i2cdetect -l
i2c-1   i2c             bcm2835 (i2c@7e804000)                  I2C adapter
i2c-2   i2c             bcm2835 (i2c@7e805000)                  I2C adapter
i2c-11  i2c             i2c_gpio                                I2C adapter
root@raspberrypi:/home/kingnan/TEMP/i2c-gpio# i2cdetect 11
WARNING! This program can confuse your I2C bus, cause data loss and worse!
I will probe file /dev/i2c-11.
I will probe address range 0x08-0x77.
Continue? [Y/n] 
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: 50 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --                         
root@raspberrypi:/home/kingnan/TEMP/i2c-gpio# i2cset -f -y 11 0x50 0 123
root@raspberrypi:/home/kingnan/TEMP/i2c-gpio# i2cget -f -y 11 0x50 0
0x7b
root@raspberrypi:/home/kingnan/TEMP/i2c-gpio# i2cset -f -y 11 0x50 0 1
root@raspberrypi:/home/kingnan/TEMP/i2c-gpio# i2cget -f -y 11 0x50 0
0x01
root@raspberrypi:/home/kingnan/TEMP/i2c-gpio# 




我有一个问题是 ，我 insmod i2c-gpio.ko 后，i2c 设备 会挂在到 哪个i2c 总线上，是 系统分配的总线吗，这时候是不是就是不用 rpi3B+  自带的 i2c 片上外设 你不是 没有作用了， 是的












# i2c 核心

 i2c 的 核心中 包含 中断机制，在 设备树 上 会有 体现，使用哪个中断， 在接受完数据 会触发 中断



 涉及到 的 核心的 设备树， 在反编译的 设备树中  
 [设备树tmp.dts](../../deviceTree/tmp.dts)
 搜索 i2c 找到 i2c@7e804000 全局搜索后 
 sources/linux-rpi-6.6.y/arch/arm/boot/dts/broadcom/bcm283x.dtsi
[bcm283x.dtsi](../../../../sources/linux-rpi-6.6.y/arch/arm/boot/dts/broadcom/bcm283x.dtsi)




i2c1: i2c@7e804000 {
    compatible = "brcm,bcm2835-i2c";
    reg = <0x7e804000 0x1000>;
    interrupts = <2 21>;
    clocks = <&clocks BCM2835_CLOCK_VPU>;
    #address-cells = <1>;
    #size-cells = <0>;
    status = "disabled";
};

这里 有 中断 2 21，  在 i2c 核心中 会使用到




i2c 控制器 内部结构


APB BUS --->  I2C CONTROLLER --->  I2C SLAVE DEVICE
![I2C CONTROLLER ](<i2c 控制器 内部结构.PNG>)




控制器 操作方法 （stm32MP157）
 1  使能时钟
 2 发送数据 
    把数据写入 tx_register （发送寄存器） 等中断发生
    中断发生后 ， 判断状态， 是否发生错误，是否得到回应信号 ACK
    把下一个数据写入 tx_register 等到中断发生， 如此循环

 3 接收数据
    设置 controller_register 接收模式，启动接收，等待中断发生
    中断发生后， 判断状态， 是否发生错误 ，读取 rx_register 得到数据


