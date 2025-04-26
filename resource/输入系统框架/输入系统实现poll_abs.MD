2025/04/26 10:10
# 输入系统实现poll 实现 abs 功能 
    分支
        dirver_raspberry_input_device_system_poll_abs_v0.2.4

    文件
        ./modules/input_device_system/input_device_system_poll_abs.c
        ./dirverModules/app_input_device_system/input_device_system_poll_abs.c

# 定义
    使用 pol 机制 监听 输入 上报的信息，并推送到 输入子系统中， 使用 main  从 输入子系统 的  event 内监听信息 上报 并解析


    __set_bit(EV_ABS, input_data->input_device->evbit);// 添加 asb 功能 
    __set_bit(INPUT_PROP_DIRECT, input_data->input_device->propbit);// 添加直接输入属性


    INPUT_PROP_DIRECT：  这个属性表示该设备是一个 直接输入设备（Direct Input Device）； 输入事件与屏幕上的位置直接相关，用户所触碰的点即为目标位置； 触摸屏、手写板

    INPUT_PROP_POINTER： 这个属性表示该设备是一个 指针设备（Pointer Device）； 输入事件与屏幕上的位置相关，但用户所触碰的点不一定为目标位置；	间接输入； 鼠标、触控板








# 流程


# 执行顺序


# 内部机制


# Makefile
# # XXXX ---------------------
XXXX-y := $(MODULES_DIR)/XXXX/XXXX.o
obj-m := XXXX.o


# 执行命令


insmod
rmmod

chmod +x main

ps -ef | grep main
kill -9 PID

ls /proc/device-tree/
ls /sys/devices/platform/
dmesg | tail
cat /proc/devices  
cd /sys/class 



# 扩展


在 github/dirveServer/dirverModules/input_device_system/input_device_system_poll_backup2.c  是 原始代码，但是错误有点多，经过改正后 是 input_device_system_poll








insmod input_device_system_poll.ko 

后 注册 自动分配  input17  和 event7
cat /proc/bus/input/devices



I: Bus=0019 Vendor=0001 Product=0001 Version=0100
N: Name="dirve-input-device"
P: Phys=input_device
S: Sysfs=/devices/platform/input_device/input/input17
U: Uniq=
H: Handlers=kbd event7 
B: PROP=0
B: EV=3
B: KEY=4000000000000 10000000



在 中看到 event7
root@raspberrypi:/dev/input# ls 
by-id  by-path  event0  event1  event2  event3  event4  event5  event6  event7  mice  mouse0  mouse1








运行 app


/app_input_device_system/app_input_device_system_poll.c

中  添加 strcpy(event_path, "/dev/input/event7"); 因为一直获取到的是 event0



root@raspberrypi:/home/kingnan/TEMP/input输入系统框架/pol# ./main /dev/my_input_device
输入设备路径: /dev/input/event0
已重置按键 0 的计数
等待按键事件（按 Ctrl+C 退出）...
字符设备事件: 按键 1: 类型=1, 代码=114, 值=1, 按下次数=4
输入事件: type=1, code=114, value=1
字符设备事件: 按键 1: 类型=1, 代码=114, 值=0, 按下次数=4
输入事件: type=1, code=114, value=0
























hexdump /dev/input/event7 -o -v -e '16/1 "%02x " "\n"'

会打印事件

0000000  015225  064013  000000  000000  115113  000011  000000  000000
95 1a 0b 68 00 00 00 00 4b 9a 09 00 00 00 00 00
0000010  000001  000162  000001  000000  015225  064013  000000  000000
01 00 72 00 01 00 00 00 95 1a 0b 68 00 00 00 00
0000020  115113  000011  000000  000000  000000  000000  000000  000000
4b 9a 09 00 00 00 00 00 00 00 00 00 00 00 00 00
0000030  015227  064013  000000  000000  014017  000001  000000  000000
97 1a 0b 68 00 00 00 00 0f 18 01 00 00 00 00 00
0000040  000001  000162  000000  000000  015227  064013  000000  000000




卸载驱动 


rmmod input_device_system_poll.ko 

后 


cat /proc/bus/input/devices


中找不到 input17

root@raspberrypi:/dev/input# ls
by-id  by-path  event0  event1  event2  event3  event4  event5  event6  mice  mouse0  mouse1











============


platform_get_resource 获取设备树中的资源信息

设备树 dts 里 通常通过 reg 属性定义硬件资源范围 （物理地址 + 大小）

platform_get_resource 正式对去设备树中 reg 属性内容，生成 resource 结构体，
驱动再 调用 ioremap 进行寄存器映射

    struct resource *platform_get_resource(struct platform_device *dev, unsigned int type, unsigned int num);

        dev: 平台设备结构体指针
        type: 资源类型，如 IORESOURCE_MEM、IORESOURCE_IRQ 等
        num: 资源编号，通常从 0 开始

    返回值： 成功返回 resource 结构体指针，失败返回 NULL

    struct resource {
        resource_size_t start; // 资源起始地址
        resource_size_t end;   // 资源结束地址
        const char *name;      // 资源名称
        unsigned long flags;   // 资源标志位
    };




    struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);// pdev 包含设备树节点的信息 ，通过 platform_get_resource 可以获取设备树中定义的内存资源信息
    void __iomem *base = ioremap(res->start, resource_size(res));
    int irq = platform_get_irq(pdev, 0);// pdev 包含设备树节点的信息，通过 platform_get_irq 可以获取设备树中定义的中断号




所以 结构体 struct qemu_ts_con 的定义必须按照 硬件寄存器 布局，与设备树中 reg 定义的 寄存器区间一一对应



要 能在 platform_get_resource(pdev, IORESOURCE_MEM, 0); 中拿到数据
设备树要定义成， 因为要从 reg 中获取数据； 这里 reg 指定了触摸屏控制器的内存区域，platform_get_resource 可以从中解析出 io->start 和 io->end。

input_device {
    compatible = "dirve,input_device";
    reg = <0x10000000 0x1000>; /* 假设触摸屏寄存器基地址为 0x10000000，大小 0x1000 */
    #address-cells = <1>;
    #size-cells = <0>;
    /* 其他按键定义 */
};

我觉的 设备树要这样写
touchscreen {
    compatible = "dirve,touchscreen";
    reg = <0x021B4000 16>;
    gpios = <&gpio 24 1>;
    interrupt-parent = <&gpio>;
    interrupts = <24 3>;
};


老师 给的 设备树 是 


/ {
	input_dev_demo {
		compatible = "100ask,input_dev_demo";
		reg = <0x021B4000 16>;
		//interrupt-parent = <&gpio1>;
		//interrupts = <5 IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING>;
		gpios = <&gpio1 5 1>;
	};
};

 要有中断 是因为 ，触屏产生中断，然后在中断中读取 寄存器的值