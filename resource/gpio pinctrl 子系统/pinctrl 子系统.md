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
         