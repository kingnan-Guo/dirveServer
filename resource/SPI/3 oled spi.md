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


make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 


dtc -I fs /sys/firmware/devicetree/base | less



sources/linux-rpi-6.6.y/drivers/spi/spidev.c
# 扩展

使用 CS MOSI CLK D/C 引脚
D/C 引脚  是控制发送的是 数据还是 命令； 0 是命令 1 是 数据,但是  D/C不属于 SPI协议的引脚，是OLED特有的引脚

ssd1306 128*64
模组手册中 

上电序列图
1 复位
2 等待电源稳定
3 发送命令 0xAE  关闭显示
4 发送命令 0xD5, 0x80  设置显示时钟分频因子
5 发送命令 0xA8, 0x3F  设置多路复用率
6 发送命令 0xD3  设置显示偏移
7 发送命令 0x8D  设置充电泵
8 发送命令 0x40  设置显示开始行
9 发送命令 0x20  设置内存地址模式
10 发送命令 0x81  设置对比度
11 发送命令 0xD9  设置预充电周期


往显存 里面写数据， 先确定是哪一页， 那一列
12 发送命令 0x00  设置低列地址
13 发送命令 0x10  设置高列地址
14 发送命令 0xB0  设置页地址
15 发送命令 0xC8  设置 COM 引脚扫描方向
16 发送命令 0xA1  设置段重定义






