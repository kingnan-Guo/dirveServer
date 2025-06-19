2025/03/29 00:10
#  TTY 行规程  相关
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

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

1 【     TTY        】
    |           |
2   |    【virtual terminal 抽象出来一层 虚拟终端， 可能有多个】[buffer0, buffer1, ...],
    ｜          ｜
    ｜          ｜
2.5【   行规程      】          （APP 通过 ioctrl 设置 行规程 ， 让行规程 处理 输入输出 、删除等 操作）
    |           |
    ｜
3 driver        driver
    ｜          ｜
    ｜          ｜
4 串口      键盘 显示设备


APP 使用 TTY 通过 驱动使用统一的 接口 访问不同的 设备
驱动成从键盘 读取到数据的时候，会把数据发送给 前台的虚拟终端，就是保存在 某个 virtual terminal 里面， 比如 保存在 virtual terminal 0
App 要显示的数据 也会 先存入 virtual terminal 1 里面，存入的 是 buffer ，等到有显示设备要显示的时候，就会从 virtual terminal 1 里面读取数据，然后显示出来


串口没有 virtual terminal  





0 当 pc  串口 连接 rpi 3b+ 的时候

1 pc vscode 终端 键盘 输入  ls
2 ps 机通过串口 发送  l
3 rpi3B+ 的uart 接收到 l
4 从硬件上得到 l 然后 发到 uart 的驱动
5 uart驱动 把 l 传到 上一层的驱动程序， 行规程 line discipline
6 行规程 line discipline  发现 l 是个 普通的字符
7 行规程 line discipline  把 l 传到 驱动
8 驱动 把 l 传到  pc 机
9 pc 机 uart  所接收到 l
10 aurt 传给 tty
11 tty 传给 virtual terminal 0
12 virtual terminal 0 保存 l
13 vscode 终端 读取 virtual terminal 0 里面的 l
14 vscode 终端 显示 l

如果需要删除 l 那么 删除发送的 也是一个 del 字符，然是 到了 arm 板 的 行规程 发现是个 删除
行规程里面 有个 buffer ， buffer 里面是 l， 接收到 删除后 就把 l 删除掉，并且会把 删除这个  del 发送回给 pc 机， pc 所接收到 然后 把 l 删掉

如果输入完 ls 这时 要传输 回车 键
这时 在 行规程 收到 回车键 ， 表示要执行命令 了 ，那么会把 行规程 里的 buffer 中的 ls， ls 上传给APP 
APP 就  shell 会查看当前 目录下有哪些文件， 会把 结果发送给 行规程，行规程 会把  结果发送给 uart 然后发送给 pc ，最终 显示在 屏幕上
