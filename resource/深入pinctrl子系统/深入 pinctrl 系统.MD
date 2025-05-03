2025/05/04 00:10
# 深入 pinctrl 系统
    分支
        dirver_raspberry_pinctrl_system_v2.5.0

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/pinctrl_system/virtual_controller_driver.c
		./dirverModules/pinctrl_system/virtual_client_driver.c
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
# # pinctrl_system  pinctrl 子系统 ---------------------		
	
pinctrl_system_dirver-y := $(MODULES_DIR)/pinctrl_system/virtual_controller_driver.o
obj-m := pinctrl_system.o
pinctrl_system_clinet-y := $(MODULES_DIR)/pinctrl_system/virtual_client_driver.o
obj-m := pinctrl_system_clinet.o



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





make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 




insmod -f pinctrl_system_dirver.ko 
-f 	强制安装驱动 ，但是这里 不需要 强制


root@raspberrypi:/home/kingnan# cd /sys/kernel/debug/pinctrl/
root@raspberrypi:/sys/kernel/debug/pinctrl# ls
3f200000.gpio-pinctrl-bcm2835  pinctrl-devices  pinctrl-handles  pinctrl-maps  virtual_pincontroller
root@raspberrypi:/sys/kernel/debug/pinctrl# cd virtual_pincontroller/
root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# ls
gpio-ranges  pinconf-groups  pinconf-pins  pingroups  pinmux-functions  pinmux-pins  pinmux-select  pins


root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# cat pins			获取引脚信息
registered pins: 4
pin 0 (pin0) 0:? pin virtual_pincontroller

pin 1 (pin1) 0:? pin virtual_pincontroller

pin 2 (pin2) 0:? pin virtual_pincontroller

pin 3 (pin3) 0:? pin virtual_pincontroller

root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# cat pingroups 	获取引脚组信息
registered pin groups:
group: pin0
pin 0 (pin0)

group: pin1
pin 1 (pin1)

group: pin2
pin 2 (pin2)

group: pin3
pin 3 (pin3)

root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# cat pinmux-pins 	获取引脚和功能映射关系， 单个引脚配置信息，当前这些 是 复用为gpio 功能
Pinmux settings per pin
Format: pin (name): mux_owner gpio_owner hog?
pin 0 (pin0): (MUX UNCLAIMED) (GPIO UNCLAIMED)
pin 1 (pin1): (MUX UNCLAIMED) (GPIO UNCLAIMED)
pin 2 (pin2): (MUX UNCLAIMED) (GPIO UNCLAIMED)
pin 3 (pin3): (MUX UNCLAIMED) (GPIO UNCLAIMED)





这路 
root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# insmod /home/kingnan/TEMP/pinctrl\ 子系统/printctrl\ 实现/pinctrl_system_clinet.ko  后 没有生效，
root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# cat pinmux-pins  还是 以前的值，新的 pinctrl_system_clinet并没有发生什么， 所以 
Pinmux settings per pin
Format: pin (name): mux_owner gpio_owner hog?
pin 0 (pin0): (MUX UNCLAIMED) (GPIO UNCLAIMED)
pin 1 (pin1): (MUX UNCLAIMED) (GPIO UNCLAIMED)
pin 2 (pin2): (MUX UNCLAIMED) (GPIO UNCLAIMED)
pin 3 (pin3): (MUX UNCLAIMED) (GPIO UNCLAIMED)
root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# cat pinconf-pins  获取到的值 本应该是  pin 0 (pin0): 0x11 0x22， 
Pin config settings per pin
Format: pin (name): configs
pin 0 (pin0): 0x0
pin 1 (pin1): 0x0
pin 2 (pin2): 0x0
pin 3 (pin3): 0x0


echo “modify config_pin virtual_i2c default pin0 0xaabb” > /sys/kernel/debug/pinctrl/virtual_pincontroller/pinctrl-configs 设置 值 到 pinctrl-configs

但是没有 pinctrl-configs

bash: /sys/kernel/debug/pinctrl/virtual_pincontroller/pinctrl-configs: Permission denied

/pinconf-config 是没有的， 所以无法写入


cat pinconf-config 所以 这报错
root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# cat pinconf-config
cat: pinconf-config: No such file or directory





# 扩展

root@raspberrypi:/sys/kernel/debug/pinctrl/virtual_pincontroller# ls
gpio-ranges  pinconf-groups  pinconf-pins  pingroups  pinmux-functions  pinmux-pins  pinmux-select  pins

pins	单个引脚信息
pingroups	引脚组信息
pinmux-functions	引脚功能信息， 在某个 function 下面支持的 groups， 支持 group 功能的 f
pinmux-pins	引脚和功能映射关系， 单个引脚配置信息
pinmux-select	引脚和功能映射关系
pinconf-pins	引脚配置信息， 
pinconf-groups	引脚组配置信息
gpio-ranges	引脚范围信息
pinctrl-configs	引脚配置信息， 可以通过写它修改制定设备，制动状态下，指定（组）引脚的 config 值
