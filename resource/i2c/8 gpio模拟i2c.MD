2025/06/11 00:10
# gpio模拟i2c
    分支
        dirver_raspberry_i2c_gpio_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

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

涉及到 i2c-gpio.c 和 设备树
会打包 linux 内核中的 i2c-gpio.c 为 i2c-gpio.ko， 然后修改设备树  指定 sda scl 具体指向 哪个 gpio

 命令：   make  modules -j4       打包 i2c-gpio.ko






如何使用 i2c-gpio


指定频率（两种方法）
    i2c-gpio,delay-us = <5>;/** 100KHz */
    clock-frequency = <100000>;/** 100KHz */


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


