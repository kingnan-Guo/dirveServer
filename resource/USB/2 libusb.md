2025/03/29 00:10
# 名称
    分支
        dirver_raspberry_XXXXX_vX.X.X

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

sources/linux-rpi-6.6.y/arch/arm/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dts
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 


dtc -I fs /sys/firmware/devicetree/base | less


# 安装 libusb 
apt-get update
apt-get install libudev-dev build-essential
apt install usbutils

# 查看是否安装成功
dpkg -l libudev-dev build-essential usbutils

libudev-dev 提供 Linux 设备管理接口（如设备热插拔监控），不包含 USB 通信功能


# libusb：专用于 USB 设备通信 的用户态库，提供设备操作 API
apt-get install libusb-1.0-0-dev


# 扩展



![alt text](<使用 libusb 读取usb 数据 .png>)
