2025/03/29 00:10
# 2 spi adc
    分支
        dirver_raspberry_spi_vX.X.X

    文件
        ./modules/app_spi/app_spi.c

        驱动
        sources/linux-rpi-6.6.y/drivers/spi/spidev.c

# 定义


使用 TLC5615 芯片

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

[text](bcm2710-rpi-3-b-plus_dac.dts)


设备树


&spi0 {
	pinctrl-names = "default";
	pinctrl-0 = <&spi0_pins &spi0_cs_pins>;
	cs-gpios = <&gpio 8 1>, <&gpio 7 1>;/** 这里 gpio 8 是  片选引脚，也就是 cs0 要连接到 spi 设备的  */

	adc0: adc@0{
		compatible = "spidev";
		reg = <0>;	/* CE0 */
		spi-max-frequency = <20000000>;
	};

	/**
	spidev0: spidev@0{
		compatible = "spidev";
		reg = <0>;	/* CE0 */
		#address-cells = <1>;
		#size-cells = <0>;
		spi-max-frequency = <125000000>;
	};

	spidev1: spidev@1{
		compatible = "spidev";
		reg = <1>;	/* CE1 */
		#address-cells = <1>;
		#size-cells = <0>;
		spi-max-frequency = <125000000>;
	};
	*/
};




sources/linux-rpi-6.6.y/drivers/spi/spidev.ko 




![rpi3B+ 引脚](<rpi3B+ 引脚.png>)



参考 sources/linux-rpi-6.6.y/tools/spi/spidev_fdx.c 开发