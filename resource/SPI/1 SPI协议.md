2025/03/29 00:10
# 名称
    分支
        dirver_raspberry_SPI_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义

spi 四个引脚
SPICLK   //SPI时钟
MOSI     //主机输出从机输入
MISO     //主机输入从机输出
CS       //片选信号


SPI发送八条数据，接收八条数据，发送和接收同时进行，即全双工通信。

1 可以设置 极性 和 相位， 在控制寄存器里面
在状态寄存器里面可以查看传输状态，是否八位全部发送完成，也可以通过状态寄存器 设置传输完成中断。
波特率寄存器 可以设置时钟频率。

发送 8 位 数据 是 如何发送出去的，就是把 数据 放到 SPI Data  Register 发送缓冲区，然后通过时钟信号，把数据一位一位的发送出去
 同时 会对 DI ， 发送 8 位 就会 采样 8 位数据， 8 位的数据也会  一位一位的 移进来， 写这个寄存器，发送完后 开始 读 这个 寄存器

# 功能


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


spi 控制器里面包含  

spi_master

每一个链接的 spi 设备 spi_device 里面有 .master


```dts


&spi0 {
	pinctrl-names = "default";
	pinctrl-0 = <&spi0_pins &spi0_cs_pins>;
	cs-gpios = <&gpio 8 1>, <&gpio 7 1>;

	spidev0: spidev@0{
		compatible = "spidev";/** compatible 必须为 spidev */
		reg = <0>;	/* CE0 */
		#address-cells = <1>;
		#size-cells = <0>;
		spi-max-frequency = <125000000>;/** 最大频率 */
	};

	spidev1: spidev@1{
		compatible = "spidev";
		reg = <1>;	/* CE1 */
		#address-cells = <1>;
		#size-cells = <0>;
		spi-max-frequency = <125000000>;
	};
};


```

sources/linux-rpi-6.6.y/drivers/spi/spidev.ko 

对于普通的 spi dev 使用自带的 spidev 就可以，但是 没有 中断

同时读写的时候要使用 ioctrl


![rpi3B+ 引脚](<rpi3B+ 引脚.png>)
