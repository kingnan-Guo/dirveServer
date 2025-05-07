2025/05/05 00:10
# 编写虚拟的gpio控制器
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义



设备树 
    /** pinctroller */
	pinctrl_virtual: virtual_pincontroller {
		compatible = "kingnan,virtual_controller_driver";
	};


	/** 虚拟的 gpio 节点 */

	gpio_virtual: virtual_gpiocontroller {
		compatible = "kingnan,virtual_gpio";
		gpio-controller;
		#gpio-cells = <2>;/**  两个整数描述 gpio */
		ngpios = <4>;/**  4个gpio */
        gpio-ranges = <&pinctrl_virtual 0 0 4>;/**  gpio 子系统 与 pinctrl 子系统 有联系， gpio 的 第0 号 引脚 映射到 pinctrl_virtual 的 0 号引脚 映射 4 个*/
	};

	gpio_virtual_led{
		compatible = "kingnan,gpio_virtual_led";
		gpio = <&gpio_virtual 0 0>;/** 使用 虚拟的 gpio 的 第 0 个引脚， 输出 模式 */
        /** pinctrl-names = "default"; 
         pinctrl-0 = <&led_pin>;  去掉这部分，在代码中使用  rang 相关的函数，让 pinctrl 子系统自动配置引脚 */ -->
	};








# 流程


 在 gpio程序中 提供 gpio_chip -> request
 在 pinctrl程序中 提供 pmxops -> request 或者 pmxops->gpio_request_enable
 


# 执行顺序


# 内部机制


# Makefile





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

# 扩展


