2025/03/29 00:10
# mmap 内存映射
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义


memory map

    1、应用程序 与驱动之间 传递数据，可以通过 read 和 write 函数进行，这涉及到 用户态的 buffer 和内核的 buffer 之间传数据；
        a: copy_from_user() 
        b: copy_to_user()

    2、应用程序 不能直接 读写 驱动程序中的buffer，需要在用户态 buffer 和 内核态 buffer 之间进行一次数据 拷贝。这种方式在数据量比较小的时候没什么问题，但是数据量比较大的时候效率就 太低了， 
       a: 例如 更新LCD 显示时，每一帧都通过 app 传给 内核，假设 LCD 采用 1024*768*4 = 3MB，那么每次更新都需要拷贝 3MB 数据，效率太低；
    
    3、改进的方式就是让 应用程序 直接读写 驱动程序的 buffer，这可以通过 mmap 实现（memory map ），把内核的 buffer 映射到用户态，让 App 在用户态直接读写；


APP 用户态程序 
    1、同一个  app 程序，在运行的时候 打印 变量 地址
        例如：int a = 0;  printf("a's address = 0x%lx, a's value = %d\n", (long unsigned int)&a, a);
        a 运行第一次 打印的值 和 运行第二次 打印的 值 是相同，打印的不是真实的 物理地址，打印的是 虚拟地址；
        b: 两个相同  app 储存变量的 物理地址 是相同的 


    2、在运行第一个程序 app1 的时候， cpu 发出的 v_addr 要 映射到 p_addr1 ， 运行 第二个程序 app2 的 时候 ，  v_addr 也要转换到 p_addr2 ，但是  p_addr1与 p_addr2 是 不同的
        a：虚拟地址 v_addr 到 物理地址  p_addr1 p_addr2 的转换需要 通过  MMU；


MMU 内存管理单元
    1、MMU 是 CPU 的一个组成部分，作用是 将  CPU 发送的 虚拟地址 转换为 物理地址；
    2、虚拟地址 和 物理地址 是一一对应的，所以 MMU 中的页表项 中的虚拟地址 和 物理地址 是一一对应的；
    3、对于不同的应用程序 转换方式不一样，所以 MMU 中有多个页表，每个应用程序对应一个页表；



应用程序 驱动程序 要访问同一块内存的时候
    1、通过 MMU
    2、驱动程序 里面 用到的 虚拟地址 drv_addr, 经过  MMU 将 drv_addr 转换为 p_addr
    3、应用程序 里面 用到的 虚拟地址 app_addr, 经过 MMU 将 app_addr 转换为 p_addr

    4、 应用程序 app 要去 访问  p_addr 的 内存， ，
        a: 首先要得到  app_addr；内核 会做 创建 虚拟地址，使用 内核函数，传入 0 或者传入一个地址 给 创建函数，如果传入的地址可以用那么，使用传入的 地址，不可以用 自动分配新地址
        b: 获得 p_addr； 如何获得 物理地址，需要驱动程序 去做！！！！！如何做？？？？？///
        e: map 映射 app_addr 到 p_addr； 这个也是 内核 提供的函数


    5、命令

            kingnan@raspberrypi:~ $ ps
                PID     TTY          TIME CMD
                999     pts/4       00:00:00 bash
                1289    pts/4       00:00:00 ps
            kingnan@raspberrypi:~ $ cat /proc/999/maps 
            557bc50000-557bd87000 r-xp 00000000 b3:02 1755                           /usr/bin/bash
            557bd9b000-557bda0000 r--p 0013b000 b3:02 1755                           /usr/bin/bash
            557bda0000-557bda9000 rw-p 00140000 b3:02 1755                           /usr/bin/bash




        root@raspberrypi:/home/kingnan/TEMP/pinctrl 子系统# ps -ef | grep main

        # 查看当前运行的进程，并过滤出包含main的进程
        root        1767    1682  0 09:39 pts/1    00:00:00 ./main /dev/my_gpio_threadedirq_0
        root        1843    1526  0 09:40 pts/0    00:00:00 grep main
        root@raspberrypi:/home/kingnan/TEMP/pinctrl 子系统# cat /proc/1767/maps
        5581a60000-5581a63000 r-xp 00000000 b3:02 254628                         /home/kingnan/TEMP/threadedirq/main
        5581a7f000-5581a80000 r--p 0000f000 b3:02 254628                         /home/kingnan/TEMP/threadedirq/main
        5581a80000-5581a81000 rw-p 00010000 b3:02 254628                         /home/kingnan/TEMP/threadedirq/main
        5589771000-5589792000 rw-p 00000000 00:00 0                              [heap]
        7f82c80000-7f82e07000 r-xp 00000000 b3:02 5543                           /usr/lib/aarch64-linux-gnu/libc.so.6
        7f82e07000-7f82e1c000 ---p 00187000 b3:02 5543                           /usr/lib/aarch64-linux-gnu/libc.so.6
        7f82e1c000-7f82e20000 r--p 0018c000 b3:02 5543                           /usr/lib/aarch64-linux-gnu/libc.so.6
        7f82e20000-7f82e22000 rw-p 00190000 b3:02 5543                           /usr/lib/aarch64-linux-gnu/libc.so.6
        7f82e22000-7f82e2f000 rw-p 00000000 00:00 0 
        7f82e32000-7f82e59000 r-xp 00000000 b3:02 5306                           /usr/lib/aarch64-linux-gnu/ld-linux-aarch64.so.1
        7f82e6b000-7f82e6d000 rw-p 00000000 00:00 0 
        7f82e6d000-7f82e6f000 r--p 00000000 00:00 0                              [vvar]
        7f82e6f000-7f82e70000 r-xp 00000000 00:00 0                              [vdso]
        7f82e70000-7f82e72000 r--p 0002e000 b3:02 5306                           /usr/lib/aarch64-linux-gnu/ld-linux-aarch64.so.1
        7f82e72000-7f82e74000 rw-p 00030000 b3:02 5306                           /usr/lib/aarch64-linux-gnu/ld-linux-aarch64.so.1
        7fe9162000-7fe9183000 rw-p 00000000 00:00 0                              [stack]
        root@raspberrypi:/home/kingnan/TEMP/pinctrl 子系统# 


        可以获得 当前进程使用的 虚拟地址空间

           5581a60000-5581a63000  放 应用程序 /home/kingnan/TEMP/threadedirq/main 的代码



内核映射现象与数据结构




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

