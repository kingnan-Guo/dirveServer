2025/05/05 00:10
# gpio驱动源码分析
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义

在 反汇编的设备树中  [反汇编设备树文件](从fdt反汇编而来的设备树.dts) 查找 gpio-controller 
查找 父级 找到 soc


查找 gpio@7e200000
root@e99a2e222cc0:/opt/sources/linux-rpi-6.6.y# grep "gpio@7e200000" * -nr


在 
sources/linux-rpi-6.6.y/arch/arm64/boot/dts/broadcom/.bcm2710-rpi-3-b-plus.dtb.dts.tmp
中 有 gpio: gpio@7e200000




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

