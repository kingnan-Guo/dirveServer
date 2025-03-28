# c_cpp_properties.json 配置 


{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",


                // "/usr/include",
                
                // "/usr/src/kernels/3.10.0-1160.119.1.el7.x86_64"
                // "/lib/modules/3.10.0-1160.119.1.el7.x86_64/build/include/",
                // "/lib/modules/3.10.0-1160.119.1.el7.x86_64/build/include/uapi",
                // "/lib/modules/3.10.0-1160.119.1.el7.x86_64/build/arch/x86/include"


                // raspberry pi arm64
                "/opt/sources/linux-rpi-6.6.y/include/**/*",               // 包含所有的内核头文件
                "/opt/sources/linux-rpi-6.6.y/arch/arm64/include/**/*",    // 包含ARM64架构的头文件
                "/opt/sources/linux-rpi-6.6.y/include/uapi/**/*"           // 包含uapi头文件


            ],
            // 定义预处理器宏
            "defines": [],
            "cStandard": "c11",
            "cppStandard": "c++14",
            "intelliSenseMode": "linux-gcc-x64"// 设置 IntelliSense 模式，通常根据编译器和架构选择
        }
    ],
    "version": 4
}




# setting.json 配置

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