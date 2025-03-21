# dirveServer
Linux 驱动 和 服务器

芯片手册
https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf


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


2025/03/07

    # platform 



2025/03/10


    设备树 相关 添加 设备节点

        分支
            dirver_raspberry_device_tree_v0.0.9
            dirver_raspberry_device_tree_v0.0.8



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
        



2025/3/17 23:56
    读取 pin 27 引脚状态使用 ,在使用 dtb 的情况下

    分支
        dirver_raspberry_input_button_v0.1.1
        dirver_raspberry_input_button_v0.1.0


   

    insmod my_chip_board_button.ko 
    insmod my_drv.ko

    cat /proc/devices  

    ls -l /dev/my_board_*

    ./main /dev/my_board_button_0
    
    rmmod my_chip_board_button.ko 
    rmmod my_drv.ko





2025/03/18 12:44    
    pinctrl 子系统


    pinctrl：
        是软件层面的 ， 是一个虚拟层 在 gpio 、 I2C 与  pin 引脚之间 由 BSP 工程师 定义  
        作用 ： 引脚复用、引脚配置




    # client device
        正常工作时：  pin-a pin=b 配置为  uart 功能
        休眠时： pin-a pin=b 配置为  gpio 功能， 并且输出 高电平

        正常工作、休眠 是 state 状态
        pin A 、pin B 是group
        pin A 、pin B 配置的 功能 是 function


    device {
        pinctrl-names = "default", "sleep";// 这个是被 有两种状态， 使用 某一个字节点 来描述 这个状态
        pinctrl-0 = <&state_0_node_a &state_0_node_b>;//子节点 ； 第 0 个状态的名字 是 default， 对应的引脚在 pinctrl-0 里定义； 位于 pincontroller 里面； 
        pinctrl-1 = <&state_1_node_a>;//子节点； 第 1 个状态 的 名字是 sleep， 对应的引脚在 pinctrl-1 里定义 ;  位于 pincontroller 里面
    }


    # pincontroller 
        没有统一格式， 但一般都是  由 pin 配置哪些引脚，然后 给引脚 配置哪些参数 配置那种模式 等，可能是上拉

    pincontroller1{
        state_0_node_a {
            function = "uart0",
            groups = "u0rxtx", "0urtscts";// 用到 哪些 引脚
        };
        state_1_node_a {
            function = "gpio",
            groups = "u0rxtx", "0urtscts";
        }
    }

    1、当设备 为 default 的时候  会 是 state_0_node_a 状态， 会把 u0rxtx", "0urtscts"   复用成  uart0 功能，  （multiplexing node）
    2、state_1_node_a 状态， 会把 u0rxtx", "0urtscts" 这一组引脚 复用成  gpio 功能  
    3、但是 rps 3B+ 可能 不适用于 分组


    pincontroller2{
        state_0_node_a {
            function = "uart0",
            groups = "u0rxtx", "0urtscts";// 用到 哪些 引脚
        };
        state_1_node_a {// 这里是把引脚 配置成 某一个状态 （configuration node）
            groups = "u0rxtx", "0urtscts";
            output-hight; 
        }
    }

    代码中如何使用
        1、pincrl 在 驱动程序中不需要管
        2、设备切换状态时，pinctrl 系统 会根据设备树， 对应的 pinctrl 就会被调用，切换设置那些 引脚，

        3、比如说 platform_device platform_dirver 的枚举过程中，内核里会有 really_prob, really_prob 会调用 我们提供 的 prob，但是 在 调用 prob 之前 会帮用户设置引脚，会根据设备树 设置成 default 或sleep 状态
        4、 有自定义的方式 设置 引脚状态    



2025/03/19 12:30

    GPIO 子系统
    要操作 GPIO 引继哦啊，先把 所用 引脚配置为 GPIO 的 功能，这通过 pinctrl 子系统来实现
    然后就可以根据配置 设置引脚方向（输入 / 输出）、读 、写

    在设备树 里定 GPIO 引脚
    在驱动代码中：
        BSP 实现 GPIO 的控制器 驱动， 驱动 直接调用 
        使用 GPIO 子系统的标准 获得 GPIO、设置方向、读取设置GPIO 值
        可以让 驱动与 板子 分离

    

    如何使用 GPIO 子系统
        1、确定使用哪个引脚，例如 stm32pm157  是去确定使用哪一组的里的哪一个引脚， 一般 是在设备树里 指定这两个 参数

                device {
                    // 可能会用多个 gpio 所以 定义 gpios
                    // <哪一组gpio(必填), 哪一个 pin (由 组 确定), flag (由 组 确定)>
                    led-gpios =<>
                }


                gpio1{
                    gpio-controller;// 用来描述 这个节点 是 gpio controller 控制器
                    #gpio-cells = <2>; // 以后要制定这一组 的某一个引脚时 要使用 两个整数 来指定； 来表述 除了 组之外 还用什么整数 来指定 ； 也就是  led-gpios 会有 三个参数， 一个 是 组 

                }




        2、假设 在 设备树已经 指定 我这设备要用哪一个引脚，在驱动程序里面 如何 获得 引脚
            a：GPIO 子系统有两套接口，基于 描述符的 （descriptor-base）、老的（legacy ， 基于整数 引脚号 的 api）；前者的函数 都有 前缀 "gpiod_"， 他使用 gpio_desc 可够题来表示一个引脚； 后者 的函数都有前缀 "gpio_", 他使用 一个整数来表示一个引脚

            b： 要操作一个引脚，首先要get引脚，然后设置方向、读值、写值


            c: 如何 获取出 值
                foo_deivce {
                    compatible = "acme,foo";

                    led-gpios = <&gpio 15 GPIO_ACTIVE_HIGH>,
                                <&gpio 16 GPIO_ACTIVE_HIGH>;

                    power-gpios = <&gpio 1 GPIO_ACTIVE_LOW>;
                }

                1: 使用 
                    struct gpio_desc *red, *gree, *power;

                    // gpiod_***  是 新的 api
                    red = gpiod_get_index(dev, "led", 0, GPIO_OUT_HIGH), ;
                    gree = gpiod_get_index(dev, "led", 0, GPIO_OUT_HIGH);

                    power = gpiod_get(dev, "power", GPIO_OUT_HIGH) ;

                    也可使用
                        gpiod_direction_input / gpiod_direction_output 设置方向


        3、获取引脚编号等


            root@raspberrypi:/# cd /sys/class/gpio/
            root@raspberrypi:/sys/class/gpio# ls
            export  gpiochip512  gpiochip566  unexport
            root@raspberrypi:/sys/class/gpio# cd gpiochip512
            root@raspberrypi:/sys/class/gpio/gpiochip512# ls
            base  device  label  ngpio  power  subsystem  uevent
            root@raspberrypi:/sys/class/gpio/gpiochip512# cat base 
            512
            root@raspberrypi:/sys/class/gpio/gpiochip512# cat label 
            pinctrl-bcm2835
            root@raspberrypi:/sys/class/gpio/gpiochip512# cat ngpio 
            54


        4、

            root@raspberrypi:/# cd /sys/class/gpio/
            root@raspberrypi:/sys/class/gpio# ls
            export  gpiochip512  gpiochip566  unexport
            gpiochip512 引脚编号
            rpi 3B+  只有一组 gpio

            所以我猜测 pin 27 的引脚编号是 512 + 27

            echo 539 > /sys/class/gpio/export

            设置输入
            echo in > /sys/class/gpio/gpio539/direction
            cat /sys/calss/gpio539/value     获取 pin 27 的值

            设置 输出
            echo 539 > /sys/class/gpio/export
            echo out > /sys/class/gpio/gpio539/direction
            echo 1 > /sys/class/gpio/gpio539/value
            echo 0 > /sys/class/gpio/gpio539/value

            echo 539 > /sys/class/gpio/unexport
         

2025/03/19 22:03
    基于 GPIO 子系统的 LED 驱动程序
    
    主要流程
        1、要使用 某个引脚，需要先把引脚 配置为 GPIO 功能，这要使用 pinctrl 子系统，只需要在设备树里指定就可以， 驱动代码 上不需要我么做任何事情
        2、 GPIO 本事需要确定 引脚，也需要在设备树里 指定
        3、 然后在驱动代码中： 获得引脚、设置方向、读值/写值



    要使用 gpio 模块 ， 要对应的引脚 通过 pinctrl 配置成 gpio 功能， 所以要现在 设备树里面 指定 pinctrl 的信息
        1、第一步设置好了 pinctrl 了
        2、第二步 把 引脚用作 gpio 功能，设置好了 pinctrl 把它 连接到 根皮哦里面去，还需要告诉gpio子系统用 哪一个引脚

    对于 pinctrl 会定义一个 节点

    这是 pinctrl 的 client device
    my_led {
        compatible = "my_board_device,my_drv";// 表示 与哪个驱动程序 挂钩
        <!-- 第一步 -->
        pinctrl_names = "default", "sleep"
        pinctrl-0 = <&XXX>

        <!-- 第二步 -->
        XXX-gpios = <gpio5 0 GPIO_ACTIVE_HIGH> // gpio 使用哪一个引脚， flag； 假设使用 gpio5 （第五组） 的第 0 个 引脚



    }

    XXX 对应的是 要在 pinctrl 下面 对应的 子节点点

    pinctrl {
        XXX {
            function = "gpio", //   .group => function 把引脚配置成某一个 功能； 如何知道吧一组引脚 配置成一个功能？？设备树要 如何写 ？？
            groups = "u0rxtx", "0urtscts";// 用到 哪些 引脚
        }
    }


    BSP 工程师 会设置好 gpio5 的值
    猜测这个代码是 设备树的 原始代码， 不用修改
    gpio5{
        gpio-controller; // 表明这是个 控制器
        #gpio-cells = <2>; // 表明需要两个整数 描述这个引脚， 具体什么还以需要参考文档； 一般来说第一个整数 是 哪一个引脚，第二个整数描述 状态
    }

    #### 
        在 myled 组件里 指定使用哪一个 引脚， 为什么也要在 XXX-gpio 里使用 哪一个引脚； XXX 中也指定了用哪些引脚 
        

        dtb 里
            在pinctrl 子系统里面 把引脚 配置成 gpio 功能
            在 gpio 属性里面， 来指定 使用哪个 引脚
        
    #####


    在驱动阶段
        1、my_led 设备节点会被编译成 platform_device， 我们要编写 对应的 platform_dirver, 
        2、在 platform_dirver 的prob 函数里面 我们要做 从 platform_device 里 获得 gpio ，要调用 gpiod_get 获得 gpio， 要注册一个 file_operations 结构
        3、 register_chrdev(file_operations) 注册 
        4、 file_operations {
            open;// 可以在open里面 使用 gpio 子系统，去把这个引脚 配置成 输出引脚； gpiod_direction_output
            write; // 使用 gpio 子系统 的函数 输出高低电平 gpio_set_value
        }


2025/03/20 00:01
    分支
        dirver_raspberry_input_button_v0.1.2
        dirver_raspberry_output_button_v0.1.2


    dtb 内容




    在 github/dirveServer/my_drv.c 中 按照例子写代码，但是 dtb 还没有配置，还没真实上 上机 实验

    我有个 疑问 是否可 可以在  rpi 3B+ 中直接替换 新的设备树？？？



    要写 两个树 对应使用设备的树  和 pinctrl 的树

    在 sources/linux-rpi-6.6.y/arch/arm/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dts 下面 

    &gpio{
        .....

        my_output_pins: my_output_pins{
            brcm,pins = <27>;// 指定需要配置的 GPIO 引脚编号 使用 27 个引脚； brcm 是 Broadcom 的缩写
            brcm,function = <1>;// 定义 GPIO 的 复用功能模式 GPIO 输出模式
        };

        my_input_pins: my_input_pins{
            brcm,pins = <26>;
            brcm,function = <0>;// GPIO 输入模式
            brcm,pull = <2>;// 上拉电阻
        }


    }



    设置 宏定义
    #define KEY_ENTER 28

    /{

        /** 输出 27 */

        // my_outputs 要与  platform_driver driver name 相匹配
        my_outputs {
            compatible = "my_outputs,my_drv";// 要与  of_device_id  的 compatible 匹配
            pinctrl-names = "default"; // 表示 默认状态
            pinctrl-0 = <&my_output_pins>;// 默认状态 的名字 是 default， 对应的引脚在 my_output_pins
            
            output_1:output_1{ // 这部分 应该是  gpio 子系统相关的 
                gpios = <&gpio 27 GPIO_ACTIVE_HIGH>; // 在gpio 子系统中 配置 27 引脚 上拉
            }
        }
        /** 输入 26 */
        my_inputs {
            compatible = "my_inputs,my_drv";
            pinctrl-names = "default";
            pinctrl-0 = <&my_input_pins>;
            input_1:input_1{
                gpios = <&gpio 26 GPIO_ACTIVE_HIGH>;
                linux,code = <KEY_ENTER>; //pin  26  高电平 触发 KEY_ENTER 事件；这个是电平触发，不是 边沿触发
                debounce-interval = <50>;// 消抖时间 50ms
            }
        }
    }


    然后对 dtb 进行打包

        可以使用
            make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs_clean
            make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs
            make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-  dtbs  V=1 
            
            
        只编译设备树 

            编译结果
            arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dtb

        


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
            my_board_n@0  my_board_n@1 my_inputs my_outputs设备树 解析之后得到的 节点


            ls /proc/device-tree 也可以看到









            ls /sys/class/gpio/gpio*








    修改 my_drv 








    报错 找不到 


    要修改 dts



        my_outputs {
            compatible = "my_outputs,my_drv";
            pinctrl-names = "default";
            pinctrl-0 = <&my_output_pins>;
            output_1-gpios = <&gpio 27 GPIO_ACTIVE_HIGH>;
        };



        my_inputs {
            compatible = "my_inputs,my_drv";
            pinctrl-names = "default";
            pinctrl-0 = <&my_input_pins>;

            input_1-gpios = <&gpio 26 GPIO_ACTIVE_HIGH>;
            input_1-linux,code = <28>;
            input_1-debounce-interval = <50>; // 消抖时间 50ms
        };




2025/03/21 21:13
    分支
        dirver_raspberry_input_button_v0.1.2

    中断


    内核线程

    进程 线程 中断 的核心： 栈

    中断正在运行的进程 线程


    在 cpu 中 有 寄存器 

    










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










# ====================================

platform 

github/dirveServer/my_chip_board_gpio.c
    1、在 my_chip_board_gpio_dirver_init 中 
        注册 my_chip_board_gpio_dirver 

    2、当前  platform_driver 跟 同名的 platform_device 匹配到之后
        就会调用 platform_driver  的 .probe 函数 my_chip_board_gpio_dirver_probe


    3、并且向上层 /opt/github/dirveServer/my_drv.c 提供了 通过  _register_device_operations 注册 p_my_device_operations 


    4、在 my_chip_board_gpio_dirver_probe 中 平台设备里面获得 资源 ， 并且记录引脚 到 global_pins 中

    5、 调用 github/dirveServer/my_drv.c 的 _device_create 函数 ，创建 device_create； 
            这样系统就会 创建好 设备节点， 应用程序才可打开 这个设备 节点 /dev/my_board_device_0


github/dirveServer/my_drv.c
    1、注册好设备节点后 ，就可以 调用 这里的 open 函数 ，然后 就 会去 /opt/github/dirveServer/my_chip_board_gpio.c  中 调用 board_init 函数 ，回去 设备引脚



github/dirveServer/my_board_n.c
    负责 提供资源

    my_device_release 用于 释放 platform_device 相关资源，防止内存泄漏

    my_device_release 不是直接调用，而是在 platform_device_unregister() 之后，当引用计数降为 0 时，内核会自动调用它。



    platform_device_register(&my_board_n_dev);
        设备注册，引用计数 +1，不会立即调用 release。

    platform_device_unregister(&my_board_n_dev);
        设备注销，引用计数 -1，当计数归零时，release my_device_release 被调用，释放资源。







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






