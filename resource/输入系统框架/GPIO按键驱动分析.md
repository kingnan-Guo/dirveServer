2025/04/09 22:19
# 名称
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
    };


}

结合中断 知道 引脚按下或者是 松开

当 发生中断 的时候 ，读取引脚的电平，会调用 input_device 的 report_key 函数，将按键事件上报给 input subsystem






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

