2025/07/20 00:10
# spi dev oled
    分支
        dirver_raspberry_spi_dev_oled_v0.4.2.4

    文件
        ./modules/app_spi_dev_oled/app_spi_dev_oled.c
        ./dirverModules/spi_dev_oled/spi_dev_oled.c
        [text](bcm2710-rpi-3-b-plus_spi_dev_oled.dts)

# 定义
 dc 依然使用 GPIO 12

# 流程


# 执行顺序


# 内部机制


# Makefile
# # spi_dev_oled 驱动 -------------------
spi_dev_oled-y := $(MODULES_DIR)/spi_dev_oled/spi_dev_oled.o
obj-m += spi_dev_oled.o


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



