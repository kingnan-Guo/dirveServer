2025/03/29 00:10
# mmap 内存映射
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义




物理地址

虚拟地址


内存映射


页表


页表项




cache
    cahe 空间有限 大概 4MB，但是 
    cpu 与 内存 之间有 一个 cache

    cpu - cache - 内存

    1、一般的 内存变量 使用 cache

    空间 

buffer




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



# 扩展

