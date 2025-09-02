2025/08/07 00:10
# Gadget 框架
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义

目的是让 linux 设备模拟成 USB 设备，从而让 pc 主机 能够识别 出 来。


# 流程

要有各种 设备描述符
配置 描述符
接口 描述符
端点 描述符


![alt text](image.png)

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

sources/linux-rpi-6.6.y/arch/arm/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dts
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 


dtc -I fs /sys/firmware/devicetree/base | less
# 扩展





# 从 赢家软件角度理解 Gadget 框架
总结来说我就是没咋理解





在 驱动中 复制出来
sources/linux-rpi-6.6.y/drivers/usb/gadget/libcomposite.ko
sources/linux-rpi-6.6.y/drivers/usb/gadget/function/usb_f_ss_lb.ko
sources/linux-rpi-6.6.y/drivers/usb/gadget/legacy/g_zero.ko





root@raspberrypi:/home/kingnan/TEMP/usb/gadget# ls
config  config.txt  g_zero.ko  libcomposite.ko  usb_f_ss_lb.ko
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# lsmod | grep dwc2
dwc2                  200704  0
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# ls /sys/class/udc/
3f980000.usb
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe libcomposite
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe usb_f_ss_lb
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe g_zero
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# dmesg | grep -i usb

将 libcomposite.ko、usb_f_ss_lb.ko 和 g_zero.ko 复制到 /lib/modules/6.6.78-v8/kernel/drivers/usb/gadget/




cp libcomposite.ko /lib/modules/6.6.78-v8/kernel/drivers/usb/gadget/ -r
cp usb_f_ss_lb.ko /lib/modules/6.6.78-v8/kernel/drivers/usb/gadget/ -r
cp g_zero.ko /lib/modules/6.6.78-v8/kernel/drivers/usb/gadget/ -r

root@raspberrypi:/home/kingnan/TEMP/usb/gadget# depmod
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe libcomposite
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe usb_f_ss_lb
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe g_zero




:q
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# ls /sys/class/udc/
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# lsmod | grep -E "libcomposite|usb_f_ss_lb|g_zero"
g_zero                 16384  0
usb_f_ss_lb            28672  0
libcomposite           77824  2 g_zero,usb_f_ss_lb
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# lsmod | grep dwc2



cp -r config.txt /boot/firmware/





将/boot/firmware/config 
# 原始的 config
```





# For more options and information see
# http://rptl.io/configtxt
# Some settings may impact device functionality. See link above for details

# Uncomment some or all of these to enable the optional hardware interfaces
dtparam=i2c_arm=on
#dtparam=i2s=on
#dtparam=spi=on

# Enable audio (loads snd_bcm2835)
dtparam=audio=on

# Additional overlays and parameters are documented
# /boot/firmware/overlays/README

# Automatically load overlays for detected cameras
camera_auto_detect=1

# Automatically load overlays for detected DSI displays
display_auto_detect=1

# Automatically load initramfs files, if found
auto_initramfs=1

# Enable DRM VC4 V3D driver
dtoverlay=vc4-kms-v3d
max_framebuffers=2

# Don't have the firmware create an initial video= setting in cmdline.txt.
# Use the kernel's default instead.
disable_fw_kms_setup=1

# Run in 64-bit mode
arm_64bit=1
kernel=kernel8.img
enable_uart=1

# Disable compensation for displays with overscan
disable_overscan=1

# Run as fast as firmware / board allows
arm_boost=1

[cm4]
# Enable host mode on the 2711 built-in XHCI USB controller.
# This line should be removed if the legacy DWC2 controller is required
# (e.g. for USB device mode) or if USB support is not required.
otg_mode=1

[cm5]
dtoverlay=dwc2,dr_mode=host

[all]

```


# 改成



```

# For more options and information see
# http://rptl.io/configtxt
# Some settings may impact device functionality. See link above for details

# Uncomment some or all of these to enable the optional hardware interfaces
dtparam=i2c_arm=on
#dtparam=i2s=on
#dtparam=spi=on

# Enable audio (loads snd_bcm2835)
dtparam=audio=on

# Additional overlays and parameters are documented
# /boot/firmware/overlays/README

# Automatically load overlays for detected cameras
camera_auto_detect=1

# Automatically load overlays for detected DSI displays
display_auto_detect=1

# Automatically load initramfs files, if found
auto_initramfs=1

# Enable DRM VC4 V3D driver
dtoverlay=vc4-kms-v3d
max_framebuffers=2

# Don't have the firmware create an initial video= setting in cmdline.txt.
# Use the kernel's default instead.
disable_fw_kms_setup=1

# Run in 64-bit mode
arm_64bit=1
kernel=kernel8.img
enable_uart=1

# Disable compensation for displays with overscan
disable_overscan=1

# Run as fast as firmware / board allows
arm_boost=1

# [cm4]
# Enable host mode on the 2711 built-in XHCI USB controller.
# This line should be removed if the legacy DWC2 controller is required
# (e.g. for USB device mode) or if USB support is not required.
# otg_mode=1

# [cm5]
# dtoverlay=dwc2,dr_mode=host

dtoverlay=dwc2,dr_mode=peripheral

[all]





```




root@raspberrypi:/home/kingnan/TEMP/usb/gadget# ls -l /lib/modules/6.6.78-v8/kernel/drivers/usb/gadget/
total 204
drwxr-xr-x 2 root root   4096 Aug 16 16:34 function
-rw-r--r-- 1 root root  22576 Sep  1 17:33 g_zero.ko
drwxr-xr-x 2 root root   4096 Aug 16 16:34 legacy
-rw-r--r-- 1 root root 109728 Sep  1 17:33 libcomposite.ko
-rw-r--r-- 1 root root  23364 Feb 25  2025 libcomposite.ko.xz
-rw-r--r-- 1 root root  38360 Aug 16 16:39 usb_f_ss_lb.ko
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# 






root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe dwc2
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# ls /sys/class/udc/
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# lsmod | grep dwc2
dwc2                  200704  0




root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe dwc2
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# ls /sys/class/udc/
3f980000.usb
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# lsmod | grep dwc2
dwc2                  200704  0
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# cat /sys/module/dwc2/parameters/dr_mode
cat: /sys/module/dwc2/parameters/dr_mode: No such file or directory
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# depmod
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe libcomposite
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe usb_f_ss_lb
\root@raspberrypi:/home/kingnan/TEMP/usb/gadget# modprobe g_zero
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# dmesg | grep -i usb
[    0.093311] usbcore: registered new interface driver usbfs
[    0.093389] usbcore: registered new interface driver hub
[    0.093467] usbcore: registered new device driver usb
[    1.818649] usbcore: registered new device driver r8152-cfgselector
[    1.820578] usbcore: registered new interface driver r8152
[    1.822497] usbcore: registered new interface driver lan78xx
[    1.824261] usbcore: registered new interface driver smsc95xx
[    1.828867] usbcore: registered new interface driver uas
[    1.830630] usbcore: registered new interface driver usb-storage
[    1.850699] usbcore: registered new interface driver usbhid
[    1.852369] usbhid: USB HID core driver
[   12.129240] dwc2 3f980000.usb: supply vusb_d not found, using dummy regulator
[   12.129639] dwc2 3f980000.usb: supply vusb_a not found, using dummy regulator
[   12.170579] usbcore: registered new interface driver brcmfmac
[   12.236110] dwc2 3f980000.usb: EPs: 8, dedicated fifos, 4080 entries in SPRAM
[  250.835863] dwc2 3f980000.usb: bound driver zero
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# ls /dev/gadget/
ls: cannot access '/dev/gadget/': No such file or directory
root@raspberrypi:/home/kingnan/TEMP/usb/gadget# 









可能的情况是 


如果 Raspberry Pi 3B+ 未通过 OTG 线缆正确连接到主机，Gadget 设备可能未完全激活，导致 /dev/gadget/ 未创建。



























# =======================================================
zero_driver

把 udc 和 dirver 绑定起来

udc 驱动包含 硬件驱动



#29 传输数据角度理解 gadget endpoint 是 核心
1 用 usb 与 pc 相连
2 创建 usb_request 翻入到 queue 中
3 udc 控制器会逐个将 queue 中的 usb_request 拿出来 一个一个执行
a 控制器比较智能，在 执行 usb_request 的时候 不需要 cpu 的参与
b 在执行完 usb_request 之后 就会 发出中断 到 cpu
4 cpu 这时候收到中断，在中断请求里面 调 回调函数， 也就是 当 usb 请求 结束的时候 中断中 会调用 usb 的回调函数


#30 传输数据角度理解 gadget loopback 驱动分析

zero.c
配置 1
f_lookback.c 数据回环
所有的回环 由 主机发起， 然后再由 主机来读数据， 可能 写入和读取的是 同一个 buffer

配置 2
f_sourcesink 数据传输 source源 提供数据 ， sink 目的
主机 - GAGET
->（sink） bufre1
<--（source）buffer


host 选择某个配置时， 默认会选择这个配置下 那些借口的第 0 个设置 altseting
当 host 发来 USB_REQ_SET_INTERFACE 请求时，可选择指定的设置
所为， 我们从 f_lookback.c 的函数

loopback_set_alt 开始分析

1 使能断点
2 分配请求
a 输入输出是 使用同一个 buffer
b host 把数据发送给 gadget， 站咋 host 角度 收到 out_req ，gadget 触发 回调函数 lookup_complete
c 收到数据后 ， 目的是把数据发回给主机，但是不能直接发送，要等主机再来读数据 ，然后把数据给到主机
d 这是 站在主机的角度，gadget 准备 in_req 输入端点， 输入请求的 buffer 为 输出 out_req，然后把输入端点的 输入请求 使用 sub_ep_queue 函数 将 in_req 放入到队列里 ，等待 host 来读取
e 如果 host 来读取数据的时候回得到之前写入的数据
f 当 host 来读取数据，调用了回调函数，回调函数里 这时 会再次创建 out_req，放入到 队里中 ，当 host 再次发来 数据，那么主机会 收到 out_req
g 在这里 主机的 usb 驱动程序 是 写、读、写、读

3

#31 传输数据角度理解 sourcesink 驱动分析
1 输入和输出是分开的
in_req
out_req


2 输入方向 host 读 gadget (源)
输入方向端点 in_eq
a 首先判断操作哪个端点 in_eq
b 分配请求 使用 ss_alloc_eq_req 创建 usb_request 和 buffer
c 设置回调函数->complete, 当 host 接收到数据的时候 ， 调用complete
d 因为 host 要读取数据 所以这时是构造数据，所以 usb_request 的 buffer 写入 0x00 或者是 有规则的数据
e 启动，这时候 gadget 就准备好数据，usb_request 放入到 queue，等待 hsot 来读， 如果 host 读到数据，那么会调用 complete 回调函数
f 在输入请求的 complete 回调函数 中 ，就简单的构造下一个usb_request 放入到 queue， 等待 host 来读取
g 所以 host 读到的都是 之前 gadget 构造好的数据


3 输出方向 host 写 gadget
输出方向 端点 out_eq
a 配置输出端点
b 使能 输出端点 、启动 输出端点
c 使用 输出端点 out_eq ， 使用 ss_alloc_eq_req 创建分配 usb_request 和 buffer
d 设置回调函数->complete, 当 host 发来数据的时候 ， 调用complete
e 因为 host 要写入数据 ，所以 usb_request 的 buffer 写入 0x55 ，等待 host 写入后 ，这里的 0x55 会变成其他数据
f 如果 host 发来数据，对于发来的数据 会先判断发来的数据是否正确 check_read_data 函数， 如果正确，会调用 complete 回调函数，
1 检查数据的方式 ，先检查是否是 0x00
g 发来的数据正确 除了会调用 complete 回调函数 ，还会再次写入 0x55 放入到队列 ， 使用 usb_ep_queue 函数 将 usb_request 放入到队列 ，等待下一次收到 host 的数据


4 创建测试程序
a app 创建两个线程 Thread1 Tread2
b Thread1 不断写入数据，写 0x00 或者 有规律的数据
c Tread2





#33 zero 编程
开发板上 安装 zero.ko
USB host 基于 libusb 编写 App
a 找到设备
b 选择配置
c 得到端点
d 读写数据

在 host 上得列出到 zero 的描述符
lsusb -v -d 0525:a4a0



modprobe命令： modprobe命令可以自动解决内核模块之间的依赖关系。当加载一个内核模块时，如果该模块依赖于其他模块，modprobe会先自动加载所需的依赖模块。
