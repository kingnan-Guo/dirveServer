2025/04/26 10:10
# 输入系统实现poll
    分支
        dirver_raspberry_input_device_system_poll_v0.2.4

    文件
        ./modules/input_device_system_poll/input_device_system_poll.c
        ./dirverModules/app_input_device_system_poll/app_input_device_system_poll.c

# 定义
    使用 pol 机制 监听 输入 上报的信息，并推送到 输入子系统中， 使用 main  从 输入子系统 的  event 内监听信息 上报 并解析

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


在 github/dirveServer/dirverModules/input_device_system/input_device_system_poll_backup2.c  是 原始代码，但是错误有点多，经过改正后 是 input_device_system_poll








insmod input_device_system_poll.ko 

后 注册 自动分配  input17  和 event7
cat /proc/bus/input/devices



I: Bus=0019 Vendor=0001 Product=0001 Version=0100
N: Name="dirve-input-device"
P: Phys=input_device
S: Sysfs=/devices/platform/input_device/input/input17
U: Uniq=
H: Handlers=kbd event7 
B: PROP=0
B: EV=3
B: KEY=4000000000000 10000000



在 中看到 event7
root@raspberrypi:/dev/input# ls 
by-id  by-path  event0  event1  event2  event3  event4  event5  event6  event7  mice  mouse0  mouse1








运行 app


/app_input_device_system/app_input_device_system_poll.c

中  添加 strcpy(event_path, "/dev/input/event7"); 因为一直获取到的是 event0



root@raspberrypi:/home/kingnan/TEMP/input输入系统框架/pol# ./main /dev/my_input_device
输入设备路径: /dev/input/event0
已重置按键 0 的计数
等待按键事件（按 Ctrl+C 退出）...
字符设备事件: 按键 1: 类型=1, 代码=114, 值=1, 按下次数=4
输入事件: type=1, code=114, value=1
字符设备事件: 按键 1: 类型=1, 代码=114, 值=0, 按下次数=4
输入事件: type=1, code=114, value=0














hexdump /dev/input/event7 -o -v -e '16/1 "%02x " "\n"'

会打印事件

0000000  015225  064013  000000  000000  115113  000011  000000  000000
95 1a 0b 68 00 00 00 00 4b 9a 09 00 00 00 00 00
0000010  000001  000162  000001  000000  015225  064013  000000  000000
01 00 72 00 01 00 00 00 95 1a 0b 68 00 00 00 00
0000020  115113  000011  000000  000000  000000  000000  000000  000000
4b 9a 09 00 00 00 00 00 00 00 00 00 00 00 00 00
0000030  015227  064013  000000  000000  014017  000001  000000  000000
97 1a 0b 68 00 00 00 00 0f 18 01 00 00 00 00 00
0000040  000001  000162  000000  000000  015227  064013  000000  000000




卸载驱动 


rmmod input_device_system_poll.ko 

后 


cat /proc/bus/input/devices


中找不到 input17

root@raspberrypi:/dev/input# ls
by-id  by-path  event0  event1  event2  event3  event4  event5  event6  mice  mouse0  mouse1