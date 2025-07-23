2025/03/29 00:10
# 名称
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

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




