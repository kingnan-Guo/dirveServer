2025/03/29 00:10
# app直接驱动at24c02
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件

        ./modules/app_i2c/app_i2c_dirver_at24c02.c
        ./dirverModules/dirverModules/XXX.c

# 定义


# 流程


写数据流程
1 发出 start 信号
2 发出 从机地址
3 发出 写哪一个地址
4 发出 读/写的数据

当前 地址 读数据流程
1 发出 start 信号
2 发出 从机地址
3 发出 读操作； （当前机身内部会保存一个 当前地址，所以不用发送地址）
4 发出 stop 信号



指定地址读
1 发出 start 信号
2 发出 从机地址
3 发出 写操作 
4 发出 写到 的  地址
5 发出 start 信号
6 发出 从机地址
7 发出 读操作 
8 读出数据
9 发出 stop 信号

也可以连续读

1 发出 start 信号
2 发出 从机地址
3 发出 写操作 
4 发出 写到 的  地址
5 发出 start 信号
6 发出  
7 发出 读操作 
8 读出数据

9 发出 读操作 
10 读出数据

11 发出 读操作 
12 读出数据

n 发出 stop 信号


# 代码

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



运行方式
写入
./at24c02 /dev/i2c-1 w "kingnan"

./at24c02 1 w "kingnan"

读数据
./at24c02 1 r

