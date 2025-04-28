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


输入子系统的流程

以 evd.c 为例

注册流程  ，就是 匹配 处理 input_dev input_handler 的流程， 最终 把处理好的结构体 放到 左右 的 链表中


左侧 input_dev 结构体，里面储存 输入的事件类型，每一种事件的 具体操作 如 x y ， 还有一个队列


右侧 input_handler 结构体 ， 里面有一些可以处理 事件的函数 ，connect ，当有新设备 连接的时候会调用 connect 函数， 将 input_dev  和 input_handler 连接起来，

    右侧也有一个 队列 ，储存 input_dev input_handler 的 信息


    在 input_handler 中 还有 event events filter match 函数
        event 是 当有事件上报的时候，会调用 event 函数
        events 是 事件队列，储存事件
        filter 是 过滤器，当有事件上报的时候，会调用 filter 函数，过滤事件
        match 是 匹配函数，当有新设备连接的时候，会调用 match 函数，匹配设备




APP 端
    open(/dev/input/event7)

    
    1、在open 中 ，创建一个  evdev_clinet 结构体，里面储存了 input_dev 和 input_handler， 一个 clinet 就代表了一个用户端
    2、 打开一个设备节点的时候 ，会有一个 evdev 与他对应， 根据 /dev/input/event7 找到 一个 dev， 并且把 这个 clinet 放入这个设备的链表， 可以由多个 应用程序打开这个设备节点
    3、app 会读取数据，最开始没有数据，所以 会休眠，应用程序 进入内核态进入驱动程序 的 evdev_read
    4、当 操作 输入设备的时候 会产生中断， 在中断中 会读数据，也会 上报数据， 使用  input_event 上报数据 ； 上报就是中爱队列中 找到 input_dev ,然后找到 对应的 input_handler ，然后调用 input_handler 的 event 函数，或者 evnets 函数进行处理，  过程中 会有多个函数传递， 最终会 唤醒 休眠的 read 


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