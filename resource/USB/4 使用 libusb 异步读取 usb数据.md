2025/08/02 00:10
# 使用 libusb 异步读取 usb数据
    分支
        dirver_raspberry_usb_mouse_async_v0.5.2

    文件
        ./modules/app_usb_mouse_async/app_usb_mouse_async.c


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

1 获取 所有的设备保存到链表
2  为 每一个 鼠标设备   ， alloc mouse,  去填充他的 transfer 结构体; 提交 transfer

3 libusb_submit_transfer 提交transfer， 在 中断函数 mouse_irq 中通过返回值 获取到数据的状态 ，打印数据

4 libusb_handle_events_timeout 的 作用是 处理事件， 设置超时 时间，但是我感觉这个 libusb_handle_events_timeout 跟上面的 transfer 没有关系， 他是处理 事件队列的， 事件队列中存放的是 transfer



![alt text](<使用 libusb 读取usb 数据 .png>)