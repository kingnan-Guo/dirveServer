2025/07/24 23:10
# 9 virtual_spi_master_new
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_virtual_spi_master/app_virtual_spi_master.c

        ./dirverModules/virtual_spi_master_new/virtual_spi_master_new.c
        [text](bcm2710-rpi-3-b-plus_virtual_spi_master_old.dts)
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

sources/linux-rpi-6.6.y/arch/arm/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dts
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 


dtc -I fs /sys/firmware/devicetree/base | less
# 扩展

