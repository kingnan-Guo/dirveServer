2025/03/29 00:10
# 串口驱动程序解析
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义


# 流程

按照调用过程 来分析

应用程序 shell 通过  某一个设备节点来 /dev/ttyS0 或 /dev/tty0 访问 驱动程序  driver

这些 设备节点 /dev/ttyS0 在内核里 有一个 cdev 与它 对应

cdev 里有 file_operations

    应用程序 shell 通过 open read write 等系统调用，来访问驱动程序

    驱动程序 通过 file_operations 来处理应用程序 shell 的请求

    应用程序 shell 通过 ioctl 来访问驱动程序, 设置 波特率等 

    驱动程序 通过 file_operations 来处理应用程序 shell 的请求

要找到 cdev 找到 file_operations


APP ：  open read write ioctl               /dev/tty
----------------------------------------


tty_io 层高提供的接口
cdev ：  file_operations = 　　{
    .open = uart_open,
    .read = uart_read,
    .write = uart_write,
    .ioctl = uart_ioctl,
}


驱动程序 ：  open read write ioctl
----------------------------------------

1 通过注册过程
 





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


注册过程








发送数据 会写入到 FIFO 这个过程会触发终端






可以从  中断 源头查看 是否产生了 中断
/proc/interrupts

cat /proc/interrupts | grep 5


 cat /proc/tty/drivers  查看串口驱动程序

cat /proc/tty/driver/serial

root@raspberrypi:/dev# ls /proc/tty/driver // 查看串口驱动程序
serial  ttyAMA      // 所有的 tty_driver
root@raspberrypi:/dev# cat /proc/tty//driver/serial  // 查看数据收发统计 信息
serinfo:1.0 driver revision:
0: uart:16550 mmio:0x3F215040 irq:71 tx:3914342 rx:3916966 oe:330 RTS|CTS|DTR
root@raspberrypi:/dev# cat /proc/tty//driver/ttyAMA 
serinfo:1.0 driver revision:
1: uart:PL011 rev2 mmio:0x3F201000 irq:99 tx:67164 rx:4426 RTS|CTS|DTR
root@raspberrypi:/dev# 


root@raspberrypi:/proc/tty# cat /proc/tty/ldiscs 
n_tty       0
n_hci      15
n_null     






