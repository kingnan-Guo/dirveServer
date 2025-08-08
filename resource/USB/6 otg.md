2025/08/07 00:10
# otg
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
# 扩展



# otg 的 原理是 将 id 引脚 连接到 GND 


# otg 硬件检测电路 

micro usb 上的 id 引脚，检测是 host 还是 device； 

1 首先在 micro usb  内部 id 引脚是有一个上拉电阻的，如果是在开发版上 的 micro usb 内部的 usb控制器 芯片会检测到 id 是高电平，
那么会工作在 device 模式；那么当前这个开发版 可以 通过 与 pc 的 usb 接口 连接 pc， pc 作为 主机
    作为 设备的时候 是 被 pc 供电
    







2 但是如果， micro usb  上 连接了 otg 转换器 ， 那么在 micro usb 上， id 会被连接到 otg 中 GND ，将 id 的电平拉低，那么 这是 这个 micro usb  接口就可以 通过 otg 插 U 盘， 读取U盘的数据，那么这个 micro usb 接口就可以作为 host 模式，读取 U 盘的数据；
U盘 被供电 ， 也就是这个 otg 接口 工作在 host 模式的 时候 要对外 供电






3 在 type-c 中  CC1 CC2 相当于 id 引脚；

CC1 CC2 内部也会有 上拉电阻， 也会 通过 otg 将 CC1 CC2 连接到 GND ，那么这个 type-c 接口就可以 作为 host 模式，读取 U 盘的数据；



# 负载的 otg 使用芯片
一些 高级的 做法，是使用 otg 的芯片，芯片中可以发出中断 给arm， 使用的 是 FUSB302MPX 芯片


判断 当前是什么 模式 是否需要供电 使用 SY628AAAC 芯片


# 检测流程
当姐pc 和 otg 的时候 ， CC1 CC2 的 电平是不一样的，所以 当 FUSB302MPX 芯片 发现 CC1 CC2  的电平发生变化的时候，会通过 SCL SDA 发出一个中断， 这个中断会 导致 FUSB302MPX 芯片的驱动被调用，会发起i2c 传输，来分辨 当前应该是什么 状态， 当 发现 如果是 工作于 host 状态的时候 会 通过另外一个引脚来 通知 SY628AAAC 芯片 ， 然后  纯软件 判定 输出 5V 电源
当前 CC1 CC2   
