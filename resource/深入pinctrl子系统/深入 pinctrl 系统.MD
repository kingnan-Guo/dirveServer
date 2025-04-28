2025/03/29 00:10
# 深入 pinctrl 系统
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义


# 流程
 pinctrl 子系统 分为 两部分

 一个是 client 的 设备树



	/** 输出 27 */

	my_outputs {
		compatible = "my_outputs,my_drv";
		pinctrl-names = "default";
		pinctrl-0 = <&my_output_pins>;
		output_1-gpios = <&gpio 27 GPIO_ACTIVE_HIGH>;
	};
	/** 输入 26 */
	my_inputs {
		compatible = "my_inputs,my_drv";
		pinctrl-names = "default";
		pinctrl-0 = <&my_input_pins>;
		input_1-gpios = <&gpio 26 GPIO_ACTIVE_HIGH>;
		input_1-linux,code = <28>;
		input_1-debounce-interval = <50>; // 消抖时间 50ms
	};


	/* my interrupt */
	my_interrupts {
		compatible = "my_interrupts,my_drv";
		pinctrl-names = "default";
		pinctrl-0 = <&my_interrupt_pins>;
		gpios = <&gpio 25 GPIO_ACTIVE_LOW>, <&gpio 24 GPIO_ACTIVE_LOW>;
		interrupt-parent = <&gpio>;
		interrupts = <24 2>, <25 2>;
		status = "okay";
	};

 一个 是 pincontroller 的 设备树


	my_output_pins: my_output_pins{
		brcm,pins = <27>;
		brcm,function = <1>;
	};

	my_input_pins: my_input_pins{
		brcm,pins = <26>;
		brcm,function = <0>;
		brcm,pull = <2>;
	};

	my_interrupt_pins: my_interrupt_pins {
		brcm,pins = <24 25>;
		brcm,function = <0>;
		brcm,pull = <2>;
	};







实现一个虚拟的  pinctrl 子系统的驱动



分为 两部分 
	1、client 的设备树， clinet 设备驱动 : virtual_client_driver.c
	2、pincontroller 的设备树，pincontroller  驱动： virtual_controller_driver.c


sources/linux-rpi-6.6.y/include/linux/pinctrl/pinctrl.h




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

