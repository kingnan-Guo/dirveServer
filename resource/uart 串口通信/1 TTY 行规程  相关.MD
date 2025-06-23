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







# 补充TTY 知识

/dev/tty0 对应前台展示的终端




串口没有虚拟终端的概念，所以不能使用open("/dev/tty0", O_RDWR)来打开串口设备文件，而是使用open("/dev/ttyS0", O_RDWR)来打开串口设备文件。
ttyS0 是真实的终端
ttyN  是 虚拟终端，是抽象出来的一个概念，可以同时有多个，比如可以同时有 tty0 tty1 tty2 tty3
while [1]; do echo -n "hello" > /dev/tty0; sleep 1; done
while [1]; do echo -n "hello tty" > /dev/tty; sleep 1; 


echo -n "hello tty" > /dev/tty; sleep 1;   这个可以发送



console 这个是  computer 计算机的 控制台， 有更大权限操作这个 computer
computer 上还连接 多个 teletype ， teletype 上面有 tty0 tty1 tty2 tty3  这些虚拟终端， 电脑上可以同时有多个虚拟终端，比如可以同时有 tty0 tty1 tty2 tty3


Terminal 远端的意思， 比如 vscode 的 终端， 串口终端， ssh 终端， 都是 terminal； 
console 是控制台 可以理解为权限更大，能查看更多信息
可以在 console 上看到内核的 打印信息
    console 是一个  Terminal， 也是一个 console
    Terminal 并不都是  console， 可以从多个 Terminal 中 选出一个 console



    

/dev/console 是一个设备节点，只能代表一个设备 是一个字符设备文件，它对应于计算机的控制台。控制台是计算机的输入输出设备，它通常用于系统管理员进行系统管理和调试。在Linux系统中，/dev/console设备文件通常指向当前系统的控制台终端，即当前正在使用的终端。

可以从内核的 cmdline 来至制定，
console=ttyS0 console=tty0

console=ttyS0 时  ，/dev/console设备文件将指向串口设备ttyS0，即串口终端。这意味着，当系统启动时，串口终端将成为控制台终端，所有的系统输出都将通过串口终端显示出来。
console=tty0 时，/dev/console设备文件将指向虚拟终端tty0，即当前正在使用的虚拟终端。这意味着，当系统启动时，虚拟终端将成为控制台终端，所有的系统输出都将通过虚拟终端显示出来。
console=tty  时 ，前台程序 的 终端 ；/dev/console设备文件将指向当前正在使用的终端。这意味着，当系统启动时，当前正在使用的终端将成为控制台终端，所有的系统输出都将通过当前正在使用的终端显示出来。


如何查看当前正在使用的终端

```bash
root@raspberrypi:/dev# cat /proc/cmdline 
coherent_pool=1M 8250.nr_uarts=1 snd_bcm2835.enable_headphones=0 cgroup_disable=memory snd_bcm2835.enable_headphones=1 snd_bcm2835.enable_hdmi=1 snd_bcm2835.enable_hdmi=0  vc_mem.mem_base=0x3ec00000 vc_mem.mem_size=0x40000000  console=ttyS0,115200 console=tty1 root=PARTUUID=c0417f62-02 rootfstype=ext4 fsck.repair=yes rootwait cfg80211.ieee80211_regdom=GB
root@raspberrypi:/dev# 
// console=ttyS0,115200 console=tty1 表示当前正在使用 tty1

echo -n "hello tty" > /dev/tty1; sleep 1;
echo -n "hello console" > /dev/console; sleep 1;


ps -ef | grep console
```
