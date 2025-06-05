# dirveServer
Linux 驱动 和 服务器

芯片手册
https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf




#  ==================== 安装搭建环境 ==============================================

# 查看使用函数 的 命令
grep -rn "register_chrdev" *


查看 
sudo yum groupinstall "Development Tools"

内核版本
uname -r

使用下面的命令安装对应的内核开发包
sudo yum install kernel-devel-$(uname -r)


这两句 没用
export C_INCLUDE_PATH=/lib/modules/$(uname -r)/build/include
export CPLUS_INCLUDE_PATH=/lib/modules/$(uname -r)/build/include




# 安装内核头文件
sudo yum reinstall kernel-devel kernel-headers
# 
rpm -qa | grep kernel


vscode 的配置
c_cpp_properties.json 配置 
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/include/c++/4.8.5/**/*",
                "/usr/include/**/*",
                // "/usr/src/kernels/3.10.0-1160.119.1.el7.x86_64/include/**/*",
                "/lib/modules/$(uname -r)/build/include/linux/**/*",
                "/lib/modules/$(uname -r)/build/include/**/*"
                // "/usr/src/kernels/3.10.0-1160.119.1.el7.x86_64/include/uapi/**/*",W
                // "/usr/include/c++/**/**/*"
            ],
            "defines": [],
            "cStandard": "c11",
            "cppStandard": "c++14",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}

最新配置

{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                // "/usr/src/kernels/3.10.0-1160.119.1.el7.x86_64/include/**/*",
                // "/usr/src/kernels/3.10.0-1160.119.1.el7.x86_64/include/uapi/**/*"
                // "/usr/src/kernels/$(uname -r)/include/**/*",
                // "/usr/src/kernels/$(uname -r)/include/uapi/**/*"
                // "/lib/modules/$(uname -r)/build/include/linux/**/*",
                // "/lib/modules/$(uname -r)/build/include/**/*",
                // "/usr/src/kernels/$(uname -r)/include/uapi/**/*"
                // "/usr/include/c++/**/**/*"
                "/usr/include",
                "/lib/modules/3.10.0-1160.119.1.el7.x86_64/build/include/",
                "/lib/modules/3.10.0-1160.119.1.el7.x86_64/build/include/uapi",
                "/lib/modules/3.10.0-1160.119.1.el7.x86_64/build/arch/x86/include"
            ],
            "defines": [],
            "cStandard": "c11",
            "cppStandard": "c++14",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}

setting.json 配置
{
    "files.associations": {
        "fs.h": "c",
        "kernel.h": "c",
        "module.h": "c",
        "init.h": "c",
        "stdio.h": "c",
        "cerrno": "c"
    },
    // "C_Cpp.errorSquiggles": "disabled"
}






/usr/include/linux/ 中的头文件主要是用于用户空间程序的标准库, 这些头文件定义了与 Linux 内核交互的接口，例如系统调用和常量。




# ==================== 编译启动 驱动模块 ===================================

# 首先未来要 手写 Makefile ； cmake 无法再 驱动中使用

# 编译驱动模块
make

ls -l devMain.ko 
# 装载 驱动模块
sudo insmod devMain.ko
sudo insmod my_led.ko
sudo insmod kingnan_led.ko

# 查看设备号
cat /proc/devices 

# 查看驱动模块是否加载成功
dmesg | tail

# 查看驱动模块是否加载成功
cat /proc/devices
    打印 ： 241 my_test_device

# 创建设备节点
sudo mknod /dev/mydev c 241 0

# 查看是否创建成功
ls -l /dev/my_device 
ls -l /dev/my_led 
ls -l /dev/my_device_0
# 测试驱动模块
./main /dev/mydev hello world


chmod +x main

./main /dev/my_led on

# 查看设备节点是否创建成功
dmesg | tail

# 删除设备节点
sudo rm /dev/mydev
sudo rm /dev/my_led

# 删除驱动模块
sudo rmmod devMain
rmmod my_led

# 查看驱动模块是否加载成功 打印中找不到  241 my_test_device
cat /proc/devices 

# 进入 系统目录 查看驱动模块
cd /sys/class 
    这里就有 my_test_device

cd /sys/class/my_test_device

ll
    lrwxrwxrwx 1 root root 0 Jan 13 22:35 my_test_device -> ../../devices/virtual/my_test_device_class/my_test_device

cd ../../devices/virtual/my_test_device_class/my_test_device

cat dev
    241:0
    根据 主次设备号 生成设备节点



# 打开内核 打印信息
echo "7 4 1 7" > /proc/sys/kernel/printk

# 关闭内核 打印信息
echo "4 4 1 7" > /proc/sys/kernel/printk

# 查看内核 打印信息
cat /proc/sys/kernel/printk

# 打印信息
dmesg | tail










# github
ssh -T git@github.com

git config --global user.name "你的用户名"
git config --global user.email "你的邮箱"


ls -al ~/.ssh
ssh-keygen -t ed25519 -C "你的邮箱"


# 查看公钥内容，复制输出内容
cat ~/.ssh/id_ed25519.pub



git fetch --all



# 写入 
./main /dev/my_test_device bacd
# 读出
./main /dev/my_test_device


# 因为 创建了两个次设备号所以可以绑定 
cat /proc/devices  
    打印
    241 my_test_device


ls -l /dev/my_test_device 
    打印 my_test_device 的次设备号是  0
    crw------- 1 root root 241, 0 Mar  4 12:51 /dev/my_test_device

所以在次设备号 1 
c 代表 字符设备
    
mknod /dev/my_test_device_1 c 241 1



ls -l /dev/my_test_device_1 
    打印
    crw-r--r-- 1 root root 241, 1 Mar  4 12:58 /dev/my_test_device_1


执行这个命令也可以 获取 my_test_device 到存入的数据 
./main /dev/my_test_device_1






gpio =============

root@raspberrypi:/opt# cd /sys/class/gpio
root@raspberrypi:/sys/class/gpio# ls
export  gpiochip512  gpiochip566  unexport

root@raspberrypi:/sys/class/gpio# echo 529 > /sys/class/gpio/export 
root@raspberrypi:/sys/class/gpio# echo 529 > /sys/class/gpio/unexport 



<!-- export ARCH=arm
export CROSS_COMPILE=aarch64-linux-gnu- -->


echo $ARCH
echo $CROSS_COMPILE




root@raspberrypi:/sys/class/gpio# usermod -aG gpio root

gpioget gpiochip0 27  # 获取 GPIO 27 的当前值
root@raspberrypi:/sys/class/gpio# gpioset gpiochip0 27=1
root@raspberrypi:/sys/class/gpio# gpioset gpiochip0 27=0


root@raspberrypi:/opt/TEMP# echo 1 | sudo tee /dev/my_led
root@raspberrypi:/opt/TEMP# echo 0 | sudo tee /dev/my_led


cat /sys/class/gpio/gpio27/value













# 运行顺序
insmod my_drv.ko
insmod my_board_n.ko

insmod my_chip_board_gpio.ko


cat /proc/devices  


ls -l /dev/my_board_device_*
chmod +x main


dmesg | tail

./main /dev/my_board_device_0 on
./main /dev/my_board_device_1 off

<!-- rmmod my_driver.ko -->



rmmod my_chip_board_gpio.ko
rmmod my_board_n.ko
rmmod my_drv.ko 

失败报错  无法 卸载驱动


# 参数解析

IORESOURCE_IRQ	start 表示 IRQ 号
IORESOURCE_MEM	start 表示 物理地址（通常是寄存器基地址）
IORESOURCE_IO	start 表示 I/O 端口地址
IORESOURCE_DMA	start 表示 DMA 通道
IORESOURCE_BUSY	资源已被占用（用于标记已使用的资源）



当 flags = IORESOURCE_IRQ 时： start 表示 IRQ 号，即中断号。
当 flags = IORESOURCE_MEM 时：start 表示 物理内存地址（用于 MMIO 设备）。
当 flags = IORESOURCE_IO 时：start 表示 I/O 端口地址（用于 x86 的 I/O 端口映射设备）。




# 设备树
使用
sources/linux-rpi-6.6.y/arch/arm/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dts
设备树


编译设备树
make dtbs V=1
fdt 是从  rpi 的 /sys/firmware 复制出来的 放到  /deviceTree 文件夹里
/sys/firmware 下的 fdt 的 dtb 文件
可以反编译

chmod -R 777 fdt


./scripts/dtc/dtc -I dts -O dtb -o tmp.dtb arch/arm/boot/dts/xxx.dts // 编译 dts 为 dtb
./scripts/dtc/dtc -I dtb -O dts -o tmp.dts arch/arm/boot/dts/xxx.dtb // 反编译 dtb 为 dts

反编译 fdt

/opt/sources/linux-rpi-6.6.y/scripts/dtc/dtc -I dtb -O dts -o tmp.dts /opt/github/dirveServer/deviceTree/fdt


# 课程资料
git clone https://e.coding.net/weidongshan/01_all_series_quickstart.git

https://download.100ask.net/videos_tutorial/embedded_linux/full_range_phase1/index.html



# 编译设备树


在
/opt/sources/linux-rpi-6.6.y/scripts/dtc/include-prefixes/arm/broadcom/bcm2710-rpi-3-b-plus.dts

文件下 

/ 添加

    my_board_n@0{
        compatible = "my_board_device,my_drv";
        pin = <GROUP_PIN(3, 1)>;
		data = "2710";
    };
    my_board_n@1{
        compatible = "my_board_device,my_drv";
        pin = <GROUP_PIN(5, 8)>;
		data = "2710";
    };


make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- clean
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- distclean
KERNEL=kernel8
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2711_defconfig

make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-  dtbs  V=1
编译 设备树


我觉得这个也
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 

打包出来的 设备树 替换掉 本来的设备树

ls /proc/device-tree/
可以看到 my_board_n@0 和 my_board_n@1

# 查看 platform

在 /sys/devices/platform/ 文件夹下可以看到 创建出来的 platform_device





ls /sys/devices/platform/my_board_n@0/driver

在 
/sys/devices/platform# 中可以看到 
my_board_n@0  my_board_n@1 两个设备树 解析之后得到的 节点


root@raspberrypi:/sys/devices/platform/my_board_n@0# ls -l driver
lrwxrwxrwx 1 root root 0 Mar 16 14:33 driver -> ../../../bus/platform/drivers/my_board_n

# 因为 平台dirver 的 名字 是 my_board_n， 在 my_chip_board_gpio.c 中 my_chip_board_gpio_dirver 的 dirve name

root@raspberrypi:/sys/devices/platform/my_board_n@0# cd ../../../bus/platform/drivers/my_board_n
root@raspberrypi:/sys/bus/platform/drivers/my_board_n# pwd
/sys/bus/platform/drivers/my_board_n


# my_board_n 平台驱动 platform_driver 支持 my_board_n@0  my_board_n@1 这两个设备
root@raspberrypi:/sys/bus/platform/drivers/my_board_n# ls
bind  module  my_board_n@0  my_board_n@1  uevent  unbind







ls /sys/class/gpio/gpio*









当前 有个 想法就是 在编译 应用层 app 的时候 也使用 sources/linux-rpi-6.6.y 中的 编译器，
1、比如说 在 main.c  中 的 #include <stdio.h> 使用的是 内核 的 #include <stdio.h> 这样就可以使用 内核的 头文件，
2、这样就可以使用 内核的 函数，比如 ioctl 函数












不知道的知识点

驱动 -----

1、互斥量 信号量
2、设备驱动模型 
    bus-dev-dir
    kobject
    kset

bus_device总线


3、设备树深入



pice 
mmc驱动
usb 驱动
输入设备驱动，input 子系统

RTC 驱动
PWM 驱动
I2C 驱动
SPI 驱动
网络驱动
字符设备驱动
块设备驱动
misc 设备驱动 ；misc 是 miscellaneous 的缩写，表示杂项设备驱动。
platform 驱动
PCI 驱动
DMA 驱动
深入中断子系统


PID 控制

uboot ----- 

根文件系统
dtb
uboot 裸机驱动

驱动大全
git clone https://e.coding.net/weidongshan/linux/doc_and_source_for_drivers.git

驱动实验
git clone https://e.coding.net/weidongshan/linux_course/linux_basic_develop.git

