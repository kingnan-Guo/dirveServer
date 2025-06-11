2025/06/11 00:10
# i2c_Adapter驱动 模拟EEPROM
    分支
        dirver_raspberry_i2c_v0.4.1

    文件

        ./dirverModules/i2c/i2c_adapter_eeprom_drv.c
        ../../dirverModules/i2c/i2c_adapter.c
 
[text](bcm2710-rpi-3-b-plus-virtual-epprom.dts)

[text](../../dirverModules/i2c/i2c_adapter_drv.c)

# 定义




# 流程


# 执行顺序


# 内部机制


# Makefile

# # i2c_adapter_eeprom_drv 驱动
i2c_adapter_eeprom_drv-y := $(MODULES_DIR)/i2c/i2c_adapter_eeprom_drv.o
obj-m += i2c_adapter_eeprom_drv.o



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


root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -l
i2c-1   i2c             bcm2835 (i2c@7e804000)                  I2C adapter
i2c-2   i2c             bcm2835 (i2c@7e805000)                  I2C adapter
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect 1
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
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# insmod i2c_adapter_eeprom_drv.ko 
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -l
i2c-1   i2c             bcm2835 (i2c@7e804000)                  I2C adapter
i2c-2   i2c             bcm2835 (i2c@7e805000)                  I2C adapter
i2c-11  i2c             i2c-bus-virtual                         I2C adapter
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect 1
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
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -F1
Error: No i2c-bus specified!
Usage: i2cdetect [-y] [-a] [-q|-r] I2CBUS [FIRST LAST]
       i2cdetect -F I2CBUS
       i2cdetect -l
  I2CBUS is an integer or an I2C bus name
  If provided, FIRST and LAST limit the probing range.
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -F 1
Functionalities implemented by /dev/i2c-1:
I2C                              yes
SMBus Quick Command              yes
SMBus Send Byte                  yes
SMBus Receive Byte               yes
SMBus Write Byte                 yes
SMBus Read Byte                  yes
SMBus Write Word                 yes
SMBus Read Word                  yes
SMBus Process Call               yes
SMBus Block Write                yes
SMBus Block Read                 no
SMBus Block Process Call         no
SMBus PEC                        yes
I2C Block Write                  yes
I2C Block Read                   yes
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -y -a 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- UU -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: 50 51 52 53 54 55 56 57 -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -y -a 11
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: 50 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cset -f -y 11 0x50 0 1
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cget -f -y 11 0x50 0
0x01
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# 








root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# rmmod i2c_adapter_eeprom_drv.ko 
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -y -a 11
Error: Could not open file `/dev/i2c-11' or `/dev/i2c/11': No such file or directory
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -l
i2c-1   i2c             bcm2835 (i2c@7e804000)                  I2C adapter
i2c-2   i2c             bcm2835 (i2c@7e805000)                  I2C adapter
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# insmod i2c_adapter_eeprom_drv.ko 
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -l
i2c-1   i2c             bcm2835 (i2c@7e804000)                  I2C adapter
i2c-2   i2c             bcm2835 (i2c@7e805000)                  I2C adapter
i2c-11  i2c             i2c-bus-virtual                         I2C adapter
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -l
i2c-1   i2c             bcm2835 (i2c@7e804000)                  I2C adapter
i2c-2   i2c             bcm2835 (i2c@7e805000)                  I2C adapter
i2c-11  i2c             i2c-bus-virtual                         I2C adapter
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cdetect -y -a 11
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: 60 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cset -f -y 11 0x50 0 123
Error: Write failed
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cset -f -y 11 0x60 0 123
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cget -f -y 11 0x60 0
0x7b
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cset -f -y 11 0x60 0 22
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cget -f -y 11 0x60 0
0x16
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cset -f -y 11 0x60 0 1
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# i2cget -f -y 11 0x60 0
0x01
root@raspberrypi:/home/kingnan/TEMP/virtual_epprom# 






# 为什么 if (msgs[i].addr == 0x60) 导致注册 0x60 的虚拟 I2C 节点？
I2C 总线探测机制（i2cdetect 的工作原理）
    
    i2cdetect 工具通过向 I2C 总线上的每个地址（通常从 0x08 到 0x77）发送探测消息来检查设备是否存在。
    
    探测消息通常是一个空的 I2C 写操作（I2C_M_WR），由 I2C 主控设备（适配器）发送到指定地址。
    
    I2C 适配器的 master_xfer 函数负责处理这些消息。如果 master_xfer 返回成功（返回消息数 num），i2cdetect 认为该地址存在设备；如果返回错误（如 -EIO），则认为该地址没有设备。
