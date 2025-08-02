2025/08/02 00:10
# 使用 libusb 同步读取 usb 数据
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_usb/app_usb_mouse.c
        ./dirverModules/dirverModules/XXX.c

# 定义


# 流程


# 执行顺序


# 内部机制


# Makefile
# 添加 libusb 链接选项
USER_LDFLAGS := -lusb-1.0 


all:

	# 交叉编译 应用层 程序
	# $(CROSS_COMPILE)gcc -o main main.c
	$(CROSS_COMPILE)gcc $(USER_CFLAGS) -o main $(SOURCES) $(MODULE_SOURCES) $(ASM_SOURCES) $(USER_LDFLAGS)
	# 反汇编
	$(CROSS_COMPILE)objdump -d main > main.dis



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


apt-get  search libusb

# 扩展





设备描述符
  └─ 配置描述符 (bConfigurationValue=1)
      └─ 接口描述符 (bInterfaceNumber=0, bInterfaceClass=0x03)
          ├─ HID 描述符           // 定义报告格式
          ├─ 端点描述符 1         // 中断 IN 端点               获取中断 IN 端点号 是为了让主机定期轮询鼠标数据。
          │   bEndpointAddress = 0x81  // IN, 端点号=1
          │   bmAttributes     = 0x03  // 中断传输
          └─ 端点描述符 2         // 中断 OUT 端点 (可选)
              bEndpointAddress = 0x01  // OUT, 端点号=1
              bmAttributes     = 0x03
