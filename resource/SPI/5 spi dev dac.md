2025/03/29 00:10
# spi_dev_dac
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_spi_dev_dac/app_spi_dev_dac.c
        ./modules/app_spi_dev_dac/app_spi_dev_dac.h
        ./dirverModules/spi_dev_dac/spi_dev_dac.c



# 定义


# 流程


# 执行顺序


# 内部机制


# Makefile
# app_spi_dev_dac 驱动
spi_dev_dac-y := $(MODULES_DIR)/spi_dev_dac/spi_dev_dac.o
obj-m += spi_dev_dac.o

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


dtc -I fs /sys/firmware/devicetree/base | less
# 扩展





sources/linux-rpi-6.6.y/drivers/spi/spidev.c



修改设备树



sources/linux-rpi-6.6.y/arch/arm/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dts



./main /dev/spi_dev_dac 2000