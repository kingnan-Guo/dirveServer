2025/03/29 00:10
# i2c_Tools
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义


# 流程


在内核里面这些  i2c_adapter 有 adapter_dev 驱动程序 ， 提供 i2c 总线上的读写能力，这些驱动程序 可以让 


APP  可以通过 i2c_dev.c 可以访问到 adapter_dev, 进而 去操作i2c 总线， 应用程序可以 直接 访问到 外面连接的 i2c 设备


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

make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 


apt install i2c-tools

i2cdetect


i2cdetect 0 检测 i2c 总线 0


root@raspberrypi:/dev# i2cdetect
Error: No i2c-bus specified!
Usage: i2cdetect [-y] [-a] [-q|-r] I2CBUS [FIRST LAST]
       i2cdetect -F I2CBUS
       i2cdetect -l
  I2CBUS is an integer or an I2C bus name

i2cdetect -l






# 扩展



进入配置菜单

启用 I²C 接口
如果安装后仍不可用，需在树莓派设置中启用 I²C：
 raspi-config
→ 选择 "Interface Options" → "I2C" → "Yes" 启用 → 重启树莓派。

也可以
加载内核模块（可选）
手动加载 I²C 驱动模块： modprobe i2c-dev


root@raspberrypi:/dev# raspi-config
root@raspberrypi:/dev# ls
autofs           dri        i2c-1    loop5         mqueue  ram13   rfkill   tty12  tty23  tty34  tty45  tty56  ttyprintk  vcs2   vcsm-cma     video10  video31
block            fd         i2c-2    loop6         net     ram14   serial0  tty13  tty24  tty35  tty46  tty57  ttyS0      vcs3   vcsu         video11  watchdog
btrfs-control    full       initctl  loop7         null    ram15   shm      tty14  tty25  tty36  tty47  tty58  uhid       vcs4   vcsu1        video12  watchdog0
bus              fuse       input    loop-control  port    ram2    snd      tty15  tty26  tty37  tty48  tty59  uinput     vcs5   vcsu2        video13  zero
cachefiles       gpiochip0  kmsg     mapper        ppp     ram3    stderr   tty16  tty27  tty38  tty49  tty6   urandom    vcs6   vcsu3        video14
cec0             gpiochip1  kvm      media0        ptmx    ram4    stdin    tty17  tty28  tty39  tty5   tty60  usb        vcsa   vcsu4        video15
char             gpiochip4  log      media1        pts     ram5    stdout   tty18  tty29  tty4   tty50  tty61  v4l        vcsa1  vcsu5        video16
console          gpiomem    loop0    media2        ram0    ram6    tty      tty19  tty3   tty40  tty51  tty62  vchiq      vcsa2  vcsu6        video18
cpu_dma_latency  hidraw0    loop1    mem           ram1    ram7    tty0     tty2   tty30  tty41  tty52  tty63  vcio       vcsa3  vga_arbiter  video20
cuse             hidraw1    loop2    mmcblk0       ram10   ram8    tty1     tty20  tty31  tty42  tty53  tty7   vc-mem     vcsa4  vhci         video21
disk             hidraw2    loop3    mmcblk0p1     ram11   ram9    tty10    tty21  tty32  tty43  tty54  tty8   vcs        vcsa5  vhost-net    video22
dma_heap         hwrng      loop4    mmcblk0p2     ram12   random  tty11    tty22  tty33  tty44  tty55  tty9   vcs1       vcsa6  vhost-vsock  video23
root@raspberrypi:/dev# i2cdetect -l
i2c-1   i2c             bcm2835 (i2c@7e804000)                  I2C adapter
i2c-2   i2c             bcm2835 (i2c@7e805000)                  I2C adapter


root@raspberrypi:/dev# i2cdetect 1
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
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --                         
root@raspberrypi:/dev# 
















设备树

ssd1306@3c {
    compatible = "ssd1306";
    reg = <0x3c>;
    status = "okay";
};






