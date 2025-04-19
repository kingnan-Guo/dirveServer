2025/04/09 22:19
# GPIO按键驱动分析
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义

平台 platform device 在probe 阶段分配/设置 /注册 input_device 




input_device 设置

/{
    input {
        input_device {
            compatible = "xxx,xxx";
            gpio = <&gpio 0 GPIO_ACTIVE_LOW>;
            gpios = <&gpio 25 GPIO_ACTIVE_LOW>, <&gpio 24 GPIO_ACTIVE_LOW>;
            interrupt-parent = <&gpio>;
            interrupts = <24 2>, <25 2>;
            linux,code = <KEY_XXX>;
        };
    };


    gpio-keys {
        compatible = "gpio-keys";
        #address-cells = <1>;
        #size-cells = <0>;
        autorepeat;

        button@4 {
            label = "enter_button";
            gpios = <&gpio 4 1>; /* GPIO4, active-low */
            linux,code = <KEY_ENTER>;
            debounce-interval = <10>;
        };

        button@17 {
            label = "power_button"; /* 可选，描述 */
            gpios = <&gpio 17 1>; /* GPIO17，active-low */
            linux,code = <116>; /* KEY_POWER 的代码 */
            debounce-interval = <15>; /* 去抖时间 15ms */
            wakeup-event-action = <1>; /* EV_ACT_ASSERTED */
        };



        button@25 {
            label = "enter_button"; /* 可选，描述 */
            gpios = <&gpio 25 1>; /* GPIO25，active-low */
            linux,code = <28>; /* KEY_ENTER 的代码 */
            debounce-interval = <10>; /* 去抖时间 10ms */
            wakeup-source; /* 支持唤醒 */
            linux,can-disable; /* 可通过 sysfs 禁用 */
        };


    };


}

结合中断 知道 引脚按下或者是 松开

当 发生中断 的时候 ，读取引脚的电平，会调用 input_device 的 report_key 函数，将按键事件上报给 input subsystem






# 流程

GPIO 输入子系统 只需要 写设备树 就可以


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


   在  
    /opt/sources/linux-rpi-6.6.y/scripts/dtc/include-prefixes/arm/broadcom/bcm2710-rpi-3-b-plus.dts



    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-  dtbs  V=1 



    编译结果
    arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dtb

    替换  
    /boot/firmware 下的 设备树



    ls /proc/device-tree/

    ls /proc/device-tree/gpio-keys

    cat /proc/bus/input/devices
    打印出

    I: Bus=0019 Vendor=0001 Product=0001 Version=0100
    N: Name="gpio26-keys"
    P: Phys=gpio-keys/input0
    S: Sysfs=/devices/platform/gpio-keys/input/input6
    U: Uniq=
    H: Handlers=kbd event5 
    B: PROP=0
    B: EV=100003
    B: KEY=10000000


    hexdump /dev/input/event5 -o -v -e '16/1 "%02x " "\n"'



    ps -ef | grep hexdump