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

