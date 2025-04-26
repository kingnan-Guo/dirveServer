2025/04/26 00:10
# 名称
    分支
        dirver_raspberry_input_device_system_poll_X.X.X

    文件
        ./modules/input_device_system_poll/input_device_system_poll.c
        ./dirverModules/app_input_device_system_poll/app_input_device_system_poll.c

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



# 扩展




hexdump /dev/input/event0 -o -v -e '16/1 "%02x " "\n"'



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



./main /dev/input/event7