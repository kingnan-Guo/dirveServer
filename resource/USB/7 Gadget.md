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



















会议 gadget



