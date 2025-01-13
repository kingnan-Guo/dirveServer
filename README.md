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







# =============== 知识点 ===============


# App 和 驱动 传输数据
copy_to_user 
copy_from_user

# 驱动和硬件
1、各个子系统的函数， 库函数
2、通过 ioremap 映射 寄存器 地址后， 在驱动程序内访问寄存器
















# 遗留问题 
    1、 老师教的 bear make 命令 还没安装好环境 ， 安装 bear 后 执行 bear make ； 然后修改生成的 文件 ，修改编译器 第一行的 cc 改为 交叉编译的工具链的 路径
    2、当前编译环境不知为何 暂时可以用，
    3、系统移植 
    4、 uboot




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

# 测试驱动模块
./main /dev/mydev hello world

# 查看设备节点是否创建成功
dmesg | tail

# 删除设备节点
sudo rm /dev/mydev

# 删除驱动模块
sudo rmmod devMain

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