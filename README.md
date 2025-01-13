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







首先未来要 手写 Makefile

ls -l main.ko

sudo insmod main.ko

dmesg | tail




# 查看驱动模块是否加载成功
cat /proc/devices