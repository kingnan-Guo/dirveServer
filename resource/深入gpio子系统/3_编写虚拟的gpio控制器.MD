2025/05/05 00:10
# 编写虚拟的gpio控制器
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义



设备树 

gpio_virtual: virtual_gpiocontroller {
    compatible = "kingnan,virtual_gpio";
    status = "okay";
    #gpio-cells = <2>;
    gpio-controller;
    #address-cells = <1>;
    #size-cells = <0>;
    ranges;
    interrupt-parent = <&gpio>;
    interrupts = <0 0>;
    reg = <0x0 0x0 0x0 0x0>;
    reg-names = "gpio";
    clocks = <&clk 0>;
    clock-names = "clk";
    reset-gpios = <&gpio 0 0>;
    reset-names = "reset";
}


gpio_virtual: virtual_gpiocontroller {
    compatible = "kingnan,virtual_gpio";
    gpio-controller;
    #gpio-cells = <2>;// 两个整数描述 gpio
    ngpios = <4>;// 4个gpio
}

gpio_virtual_led{
    compatible = "kingnan,gpio_virtual_led";
    gpio = <&gpio_virtual 0 0>;// 使用 虚拟的 gpio 的 第 0 个引脚， 输出 模式
}



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

