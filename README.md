# dirveServer
Linux 驱动 和 服务器


# ============= 日志记录 ==================

2025/01/13 19：51
传输数据 在 App 与 驱动 之间 ： dirveServer_tranfer_data_01
    1、使用 copy_to_user 和 copy_from_user 传输数据， app 读取 驱动内的信息
    2、初始化 : class_create 创建设备类， device_create 创建设备节点; 卸载时： device_destroy 销毁设备节点， class_destroy 销毁设备类
    3、测试驱动模块
        # 写入
        ./main /dev/my_test_device helloworld
        # 读取
        ./main /dev/my_test_device
        # 删除设备节点
        sudo rm /dev/my_test_device
        # 删除驱动模块
        sudo rmmod devMain

        # 查看驱动模块是否加载成功 打印中找不到  241 my_test_device
        cat /proc/devices


2025/01/13 23:00
App使用驱动的四种方式 ： dirveServer_tranfer_data_02
    1、非阻塞方式（查询）
    2、阻塞方式 休眠-唤醒
    3、poll 方式 （闹钟） ；           有没有 epoll 方式？？ 是有的 支持poll 就支持 epoll
    4、异步 通知 方式 （中断）
        a:保存现场
        b: 分辨中断源
        c: 进行不同处理, 调用中断处理函数
        d: 恢复现场
 


2025/2/06 22:29











# =============== 知识点 ===============


# App 和 驱动 传输数据
copy_to_user 
copy_from_user

# 驱动和硬件
1、各个子系统的函数， 库函数
2、通过 ioremap 映射 寄存器 地址后， 在驱动程序内访问寄存器



















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
ls -l /dev/my_test_device 
ls -l /dev/my_led 

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
git fetch origin


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
