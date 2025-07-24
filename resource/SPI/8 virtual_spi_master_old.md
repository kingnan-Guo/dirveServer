2025/03/29 00:10
# 名称
    分支
        dirver_raspberry_virtual_spi_master_v0.4.2.6

    文件
        ./modules/app_virtual_spi_master/app_virtual_spi_master.c
        ./dirverModules/virtual_spi_master/virtual_spi_master.c
        [text](bcm2710-rpi-3-b-plus_virtual_spi_master_old.dts)


# 定义
老方法 中 最核心的 是 
master 中的 transfer 函数

# 流程

spi_master{
    queue:[
        {
            spi_msg:{
                transfer:[
                    spi_transfer1,
                    spi_transfer2,
                    spi_transfer3,
                ]
            }
        }
    ]
}

1 会根据 spi_device 找到 spi_master
2 把 msg 放入到 spi_master 的 queue 中
3 sheduler_work 调度
    3.1 从Queue 取出 msg
    3.2 启动传输
    3.3 等待传输完成 -> 中断
    3.4 唤醒 下面的 4
4 等到 msg 完成

# 执行顺序


# 内部机制




	virtual_spi_master {
		compatible = "virtual_spi_master";
		pinctrl-names = "default";
		pinctrl-0 = <&virtual_spi0_pins &virtual_spi0_cs_pins>;
		cs-gpios = <&gpio 26 1>;
		status = "okay";
		#address-cells = <1>;
		#size-cells = <0>;


		virtual_spi_dev: virtual_spi_dev@0{
			compatible = "spidev";
			reg = <0>;	/* CE1 */
			spi-max-frequency = <10000000>;
		};
	};




	virtual_spi0_pins: virtual_pins{
		brcm,pins = <19 20 21>;
		brcm,function = <4>; /* alt0 */
	};
	virtual_spi0_cs_pins: virtual_cs_pins{
		brcm,pins = <16 13>;
		brcm,function = <1>; /* output */
	};


# Makefile

# # virtual_spi_master 虚拟 SPI 主设备驱动 -------------------
virtual_spi_master-y := $(MODULES_DIR)/virtual_spi_master/virtual_spi_master.o
obj-m += virtual_spi_master.o



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

sources/linux-rpi-6.6.y/arch/arm/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dts
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 


dtc -I fs /sys/firmware/devicetree/base | less
# 扩展








root@raspberrypi:/home/kingnan/TEMP/virtual_spi_master# cd /sys/bus/platform/
root@raspberrypi:/sys/bus/platform# ls
devices  drivers  drivers_autoprobe  drivers_probe  uevent
root@raspberrypi:/sys/bus/platform# cd devices/
root@raspberrypi:/sys/bus/platform/devices# ls
 3f004000.txp              3f200000.gpio         3f215000.aux          3f902000.hdmi    bcm2835-isp      cpufreq-dt                phy                   soc                    soc:nvmem:nvmem_cust
 3f007000.dma-controller   3f200000.gpiomem      3f215040.serial       3f980000.usb     bcm2835-power   'Fixed MDIO bus.0'         raspberrypi-cpufreq   soc:firmware           soc:nvmem:nvmem_otp
 3f00b840.mailbox          3f202000.mmc          3f300000.mmcnr        3fc00000.v3d     bcm2835-wdt      fixedregulator_3v3        raspberrypi-hwmon     soc:firmware:clocks    soc:power
 3f00b880.mailbox          3f204000.spi          3f400000.hvs          arm-pmu          cam1_regulator   fixedregulator_5v0        reg-dummy             soc:firmware:expgpio   timer
 3f100000.watchdog         3f206000.pixelvalve   3f804000.i2c          bcm2835_audio    cam_dummy_reg    hdmi-audio-codec.0.auto   regulatory.0          soc:firmware:vcio      vcsm-cma
 3f101000.cprman           3f207000.pixelvalve   3f805000.i2c          bcm2835-camera   chosen           kgdboc                    serial8250            soc:gpu                virtual_spi_master
 3f104000.rng              3f212000.thermal      3f807000.pixelvalve   bcm2835-codec    clocks           leds                      snd-soc-dummy         soc:nvmem
root@raspberrypi:/sys/bus/platform/devices# cd virtual_spi_master
root@raspberrypi:/sys/bus/platform/devices/virtual_spi_master# ls
driver  driver_override  modalias  of_node  power  spi_master  subsystem  supplier:platform:3f200000.gpio  uevent
root@raspberrypi:/sys/bus/platform/devices/virtual_spi_master# 


root@raspberrypi:/sys/bus/platform/devices/virtual_spi_master# ls /sys/bus/spi/
devices  drivers  drivers_autoprobe  drivers_probe  uevent
root@raspberrypi:/sys/bus/platform/devices/virtual_spi_master# ls /sys/bus/spi/devices/
spi0.0  spi0.1  spi3.0



root@raspberrypi:/home/kingnan/TEMP/virtual_spi_master# ./main /dev/spidev3.0 
Usage: ./main /dev/spidevB.D <val>
root@raspberrypi:/home/kingnan/TEMP/virtual_spi_master# ./main /dev/spidev3.0 100
Pre val = 3037
root@raspberrypi:/home/kingnan/TEMP/virtual_spi_master# ./main /dev/spidev3.0 200
Pre val = 100
root@raspberrypi:/home/kingnan/TEMP/virtual_spi_master# ./main /dev/spidev3.0 200
Pre val = 200
root@raspberrypi:/home/kingnan/TEMP/virtual_spi_master# ./main /dev/spidev3.0 200
Pre val = 5851
root@raspberrypi:/home/kingnan/TEMP/virtual_spi_master# ./main /dev/spidev3.0 200
Pre val = 200
root@raspberrypi:/home/kingnan/TEMP/virtual_spi_master# 





