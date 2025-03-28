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


