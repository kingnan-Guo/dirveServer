2025/06/14 00:10
# i2c 驱动 oled ssd1306
    分支
        dirver_raspberry_i2c_oled_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义


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




&i2c1 {
	pinctrl-names = "default";
	pinctrl-0 = <&i2c1_pins>;
	clock-frequency = <100000>;

	at24c02@50 {
		compatible = "kingnan,i2c_dev";
		reg = <0x50>;
		pagesize = <8>;
		status = "okay";
	};

	/* 添加 SSD1306 设备 */
    oled@3c {
        compatible = "kingnan,oled"; // 与驱动中的 of_match 兼容
        reg = <0x3c>; // I2C 地址
        status = "okay";
        /* 可选：指定显示参数 */
        display-width = <128>;
        display-height = <64>;
    };




};


曾经是这个

	ssd1306@3c {
		compatible = "solomon,ssd1306fb";
		reg = <0x3c>;
		solomon,width = <128>;
		solomon,height = <64>;
		solomon,page-offset = <0>;
		solomon,com-invdir;
		solomon,segment-no-remap;
		solomon,prechargep2 = <2>;
		status = "okay";
	};







root@raspberrypi:/home/kingnan/TEMP/ssd1306# ls
bcm2710-rpi-3-b-plus.dtb  ssd1306_dirver.ko
root@raspberrypi:/home/kingnan/TEMP/ssd1306# i2cdetect -l
i2c-1   i2c             bcm2835 (i2c@7e804000)                  I2C adapter
i2c-2   i2c             bcm2835 (i2c@7e805000)                  I2C adapter
root@raspberrypi:/home/kingnan/TEMP/ssd1306# i2cdetect 1
WARNING! This program can confuse your I2C bus, cause data loss and worse!
I will probe file /dev/i2c-1.
I will probe address range 0x08-0x77.
Continue? [Y/n] 
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- 3c -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: 50 51 52 53 54 55 56 57 -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --                         
root@raspberrypi:/home/kingnan/TEMP/ssd1306# insmod ssd1306_dirver.ko 
root@raspberrypi:/home/kingnan/TEMP/ssd1306# i2cdetect -l
i2c-1   i2c             bcm2835 (i2c@7e804000)                  I2C adapter
i2c-2   i2c             bcm2835 (i2c@7e805000)                  I2C adapter
root@raspberrypi:/home/kingnan/TEMP/ssd1306# 
root@raspberrypi:/home/kingnan/TEMP/ssd1306# i2cdetect 1
WARNING! This program can confuse your I2C bus, cause data loss and worse!
I will probe file /dev/i2c-1.
I will probe address range 0x08-0x77.
Continue? [Y/n] 
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- UU -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: 50 51 52 53 54 55 56 57 -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --                         
root@raspberrypi:/home/kingnan/TEMP/ssd1306# dmesg | tail
[  140.929012] hwmon hwmon1: Undervoltage detected!
[  148.993503] hwmon hwmon1: Voltage normalised
[  151.009742] hwmon hwmon1: Undervoltage detected!
[  155.041816] hwmon hwmon1: Voltage normalised
[  157.058013] hwmon hwmon1: Undervoltage detected!
[  157.972419] ssd1306_dirver: loading out-of-tree module taints kernel.
[  157.973431] /opt/github/dirveServer/dirverModules/i2c/i2c_dirver_ssd1306.c my_i2c_dirver_init 147
[  157.973591] /opt/github/dirveServer/dirverModules/i2c/i2c_dirver_ssd1306.c my_i2c_driver_probe 83
[  165.122225] hwmon hwmon1: Voltage normalised
[  167.138450] hwmon hwmon1: Undervoltage detected!
root@raspberrypi:/home/kingnan/TEMP/ssd1306# 