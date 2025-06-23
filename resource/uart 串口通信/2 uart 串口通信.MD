2025/03/29 00:10
# linux uart 串口 应用编程
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c
 

        # 发送接收数据 --------- 这程序直接操作 自带的 uart 驱动
        # 将 rx 和 tx 接到一起 然后 发送数据的时候就可以接收数据
        ./modules/app_uart/app_uart_send_recv.c


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


0       APP                     （会调用 open/read/wite/   ioctrl ）

1 [     TTY        ]
    |           |
2   |        [virtual terminal 抽象出来一层 虚拟终端， 可能有多个][buffer0, buffer1, ...],
    ｜          ｜
    ｜          ｜
2.5【   行规程      】          （APP 通过 ioctrl 设置 行规程 ， 让行规程 处理 输入输出 、删除等 操作）
    |           |
    ｜
3 driver        driver
    ｜          ｜
    ｜          ｜
4 串口      键盘 显示设备






    [hardware]                                      [kernel]
【  Terminal  -> phyical line -> UART 】---> 【  UART driver ->   line discipline  ->  tty  】 -> 【 APP  （ARM 板子）   】

在linux 系统中， 操作设备的统一接口 就是  open / ioctl / read / write


行规程 会直接处理一些操作，比如 字符的 回传

也可以 配置 原始 模式 ， 这样就不会 通过 行规程 处理了，数据直接到达 APP， APP 直接处理

对于 UART ，又在 ioctl 上封装了 很多函数，主要用来设置 行规程
所以对于 UART ， 编程的 套路是
    1. open
    2. ioctl： 设置 行规程 ， 比如 波特率、 数据位、停止位、检验位、 RAW 模式 ，   也可以设置  一有数据就返回， 只有在输入 回传的时候哦才会 再  往后传到  APP 
    3. read
    4. write
    5. close





tc: terminal control
cf: control flag

tcgetattr       get terminal attributes 获得终端的属性
tcsetattr       set terminal attributes 设置终端的属性
tcsendbreak     send break 发送中断
tcflush        flush input/output buffer 清空终端未完成的 输入输出缓冲区
cfsetispeed       set input baud rate 设置输入波特率
cfsetospeed     set output baud rate 设置输出波特率
cfmakeraw       make terminal into a raw mode 将终端设置为原始模式
cfsetspeed      set input and output baud rate 设置输入输出波特率
cfgetispeed     get input baud rate 获得输入波特率





# 第一个测试驱动  本机 发送接收数据
就是把 rpi 3B+  的 RX TX 接到一起




root@raspberrypi:/dev# ls /dev/serial*
/dev/serial0
root@raspberrypi:/dev# ls -l /dev/serial0
lrwxrwxrwx 1 root root 5 Jun 19 00:17 /dev/serial0 -> ttyS0
root@raspberrypi:/dev# 


./main /dev/ttyS0 