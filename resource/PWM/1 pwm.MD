2025/10/18 00:10
# 名称
    分支
        dirver_raspberry_pwm_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义


# 流程



1 已经在 编辑了 
TEMP/usb/gadget/config.txt
 文件 最后一行 添加

dtoverlay=dwc2,dr_mode=peripheral

2 ls /sys/class/pwm/pwmchip0/



root@raspberrypi:/home/kingnan/TEMP# cd /sys/class/pwm/
root@raspberrypi:/sys/class/pwm# ls
pwmchip0
root@raspberrypi:/sys/class/pwm# cd pwmchip0
root@raspberrypi:/sys/class/pwm/pwmchip0# ls
device  export  npwm  power  subsystem  uevent  unexport
root@raspberrypi:/sys/class/pwm/pwmchip0# ls
device  export  npwm  power  subsystem  uevent  unexport
root@raspberrypi:/sys/class/pwm/pwmchip0# ls
device  export  npwm  power  subsystem  uevent  unexport
root@raspberrypi:/sys/class/pwm/pwmchip0# echo 0 > export
root@raspberrypi:/sys/class/pwm/pwmchip0# ls
device  export  npwm  power  pwm0  subsystem  uevent  unexport
root@raspberrypi:/sys/class/pwm/pwmchip0# cd pwm0
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# ls
capture  duty_cycle  enable  period  polarity  power  uevent
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# echo 1000000 > period
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# echo 500000 > duty_cycle
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# echo 1 > enable
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# echo 300000 > duty_cycle
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# echo 1 > enable
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# echo 100000 > duty_cycle
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# echo 800000 > duty_cycle
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# echo 900000 > duty_cycle
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# echo 200000 > duty_cycle
root@raspberrypi:/sys/class/pwm/pwmchip0/pwm0# echo 0 > enable



实现了 sysfs 操作 pwm


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



