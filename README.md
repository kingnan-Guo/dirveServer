# dirveServer
Linux 驱动 和 服务器



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









/usr/include/linux/ 中的头文件主要是用于用户空间程序的标准库, 这些头文件定义了与 Linux 内核交互的接口，例如系统调用和常量。





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

# 测试驱动模块
./main /dev/mydev hello world

# 查看设备节点是否创建成功
dmesg | tail

# 删除设备节点
sudo rm /dev/mydev

# 删除驱动模块
sudo rmmod devMain

# 查看驱动模块是否加载成功 打中找不到  241 my_test_device
cat /proc/devices 
    


