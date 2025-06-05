2025/06/06 00:10
#  编写设备驱动 i2c_dirver_example
    分支
        dirver_raspberry_i2c_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/i2c/i2c_dirver.c

# 定义


# 流程


# 执行顺序


# 内部机制


# Makefile
# # i2c 驱动框架
# /dirveServer/dirverModules/i2c/i2c_dirver.c
i2c_dirver-y := $(MODULES_DIR)/i2c/i2c_dirver.o
obj-m += i2c_dirver.o



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



i2cset -y 1 0x50 0x00 0xAA  # 向地址 0x00 写入 0xAA
i2cget -y 1 0x50 0x00       # 从地址 0x00 读取数据

i2cget -y 1 0x51 0x00 

# 扩展
git clone https://e.coding.net/weidongshan/linux_course/linux_basic_develop.git



sources/linux-rpi-6.6.y/drivers/i2c/i2c-dev.c

sources/linux-rpi-6.6.y/drivers/misc/eeprom/eeprom.c


