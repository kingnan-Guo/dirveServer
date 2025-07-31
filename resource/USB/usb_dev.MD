2025/07/25 00:10
# usb_dev
    分支
        dirver_raspberry_usb_dev_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义


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

sources/linux-rpi-6.6.y/arch/arm/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dts
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 


dtc -I fs /sys/firmware/devicetree/base | less



lsusb -v 
# 扩展




 





低速
 低速情况下不用 检测端点状态， 因为支持低速就 不支持高速， 所以端点状态是固定的
 D- 连接 VCC 所以 在插到电脑上的时候， 电脑会检测到设备， D- 是高电平 所以是 低速 


    J 信号
        D+  低电平
        D-  高电平


    K 信号
        D+  高电平
        D-  高电平

    低速设备 D- 上拉
    IDLE 状态 是 J 状态



    1 低速设备平时
        D- 是 高电平
        D+ 是 低电平

        也就是 低速的 IDLE 状态是 K 状态

    2 SOP ： 要发起传输的时候，SOP： D- 是 低电平，D+ 是 高电平， 这个转换到 K 状态的 的过程是SOP ，也就是 K
    3  SYNC （同步字段）： 格式为 3对K/J ， 2个 K
        K J K J K J K K

全速


    J 信号
        D+  高电平
        D-  低电平


    K 信号
        D+  低电平
        D-  高电平

    全速设备 D- 下拉
    IDLE 状态 是 J 状态
    SEO 



高速

    J 信号
        D+  高电平
        D-  低电平


    K 信号
        D+  低电平
        D-  高电平

    D+ 与 D- 的电平差别不大 约 400mv

    IDLE 状态： D+ 和 D- 均保持 低电平（接近0V）
    SE0 状态： D+ 和 D- 均保持  低电平（接近0V）

    chirp 信号：
        高速模式握手过程中，设备和主机使用 chirp j 和 chirp k 信号
        首先 低电平复位设备
        chirp K ： 设备发送 D-高 、D+ 低 ，持续一段时间表明支持高速模式
        chirp J/K ： 主机相应交替 的 J 和 K 信号，确认切换到高度模式


    用途

        J/K 状态用于数据传输和 同步
        chirp J/K 用于速度协商 和 模式切换



    工作在 高速模式的时候 ，会把 上拉电阻断开；
    当一个设备工作在高速模式的时候 ，两边的 D+ D- 电压差很小，所以可以检测到设备是否工作在高速模式， 而且 两边的 D+ D-  都会接一个 45欧姆的 下拉电阻;两边 45 欧姆的电阻 是用来消除 反射信号的，
    

    # 如何检测 高速设备 拔出
    1 发送 出去的 信号 会被 反射回来，如果没有接下拉电阻那么 反射信号 与当前信号叠加 幅度会很大，所以可以检测到 反射信号， 反射信号 大于 1.5V 的时候，可以认为设备已经拔出



    连接的步骤 
    1 设备上电
    2 开始 处于 IDLE
    3 主机发送 SE0， 设备复位
    4 主机发送 chirp K ，设备检测到 chirp K 信号，开始 chirp J/K 信号，切换到 高速模式
    5 主机发送



# 完整的数据包
1 SOP （Start of Packet，数据包开始）： HUb 驱动 D+、D- 这两条线 从 IDLE 状态变为 K 状态。SOP 中的 K 状态 就是 SYNC 信号的 第一位 数据 ； 变化的瞬间是 SOP 
2 SYNC （同步字段）： 格式为 3对K/J ， 2个 K ; 在USB 中 ，两根线线传播 一部分 SYNC 同步信号， 接收到同步信号 可计算出 周期 T，用于计算传输速率; 在传输过程中 使用两条信号的电平差 计算数据
3 packet content：
    a PID（Packet Identifier，数据包标识符）
    b 地址
    c 帧号
    d 数据
    e CRC（Cyclic Redundancy Check，循环冗余校验）
6 EOP （End of Packet，数据包结束）： 由数据的发送方发出 EOP ，数据发送方驱动 D+ 、D-这两条线路， 先设为 SEO 状态并维持 2 位事件，在设置为 J 状态并 维持 1 位 事件，最后D+ D- 变为高阻状态，这是由 线路上的 上下拉电阻是的 总线进入 IDEL 状态


Differential（差分信号）



# 数据传输
 数据 0 信号在 J 和 K 之间翻转
 数据 1 保持在 当前状态，就是 J K  信号不变


# 设备检测 
    主机通过 检测 D+ 或 D- 的上拉电阻 J 状态 判断设备是低速还是 全速
    高速设备 通过 chirp J/K 信号 切换到 高速模式




    J/K状态用于表示数据包的开始、结束（EOP）以及其他控制信号。
    EOP（单端零，SE0）由D+/D-均低表示，持续一定时间




# SE0的定义
SE0状态：D+和D-同时为低电平（0V），即没有差分信号。


# NRZI 编码（反向不归零 ）
NRZI编码是一种用于数字通信的编码方式，其中数据位1被编码为0，数据位0被编码为1。这种编码方式可以有效地减少信号中的噪声，并提高信号的传输速率。
NRZI 的编码方位。为， 对于数据 0， 波形翻转 也就是从 K 到 J 或者 J 到 K； 对于数据 1 ，波形 不变， 也即是 K 到 K， 或者 J 到 J
发生变化就是 0， 不变就是 1 

但是在这个过程中 如果 传输 100 个 1。那么正常来说 波形要保持不变，也就是 D+ D- 之间 电压差 不变，但是 由于 信号反射， 波形会发生变化， 也会因为 晶振 不准的时候 收到 99 个1 ，所以 NRZI 编码方式 在传输 100 个 1 的时候， 每 6 个 1  会插入一个 0 ， 也就是 1 1 1 1 1 1 0 1 1 1 1 1 1 0
设备收到 6 个1 的 时候 会 舍弃掉 后面的 0






批量传输


发送数据
Token packet（令牌 包 ）
    sop  sync pid(out/in) addr endp（端点） crc eop

Data packet（数据 包）
    sop sync pid(Data0/Data1)  data             crc eop



Handshake packet（握手 包）     ACK 设备回应
    sop sync pid(ACK/NACK/STALL) crc eop




host 想要读取 device 数据
    1 host 发送 Token packet 请求 device 发送数据
        sop sync pid(in) addr endp crc eop
    2 device 收到 Token packet 请求， 发送 Data packet 数据
        sop sync pid(Data0/Data1)  data             crc eop
    3 host 收到 Data packet 数据， 发送 Handshake packet 确认收到数据
        sop sync pid(ACK) crc eop

读取很多数据那么久多次 读取
Data0 Data1 会交替

# 中断传输
设备是没有能力通知设备的
每隔一段时间就去访问设备
使用的是 中断事务，我感觉使用的是 定时中断



# 实时 的发送数据 iso
还是批量传输，但是没有 握手机制
不在乎数据是否正确



pid 是有 优先级的 ， 优先级高的 pid 会被优先处理，数字越小优先级就越高，物理原理是二进制  0 低电平 会 拉低电平，谁的数字越小  高位 0 越多，所以 数字越小 优先级越高




# 控制传输由 多个事务 组成 
1 发起建立事务
2 发起数据传输事务
3 状态 事务

所有的 usb传输都是 由host发起
pid = setup 的时候 通知 device 开始一个 控制传输； 没有方向

控制传输 也有三个过程
1 token：  通知设备 开始传输了      sop sync pid(setup) addr endp crc eop
2 Data ：    数据事务                    sop sync pid(Data0/Data1)  data             crc eop
 
3 Handshake ：     握手包               sop sync pid(ACK) crc eop


4 Data stage 数据过程。是一个批量传输，由 一个或者多个批量事务组成
   a 如果要在 读数据的时候读取 设备描述符 信息， host 发送 token packet 请求 device 发送数据
   b 每一个 事务 中包含  token data Handshake  
        1 sop sync pid(out) addr endp crc eop  ｜ sop sync pid(Data0)  data  crc eop ｜ sop sync pid(ACK) crc eop ｜sop sync pid(out) addr endp crc eop ｜  sop sync pid(Data1)  data  crc eop ｜    sop sync pid(ACK) crc eop
        
        
   c 事务结束后， host 发送 token packet 请求 device 发送数据， 重复 事务过程，直到 host 读取完数据
   

5 status stage 最后状态过程。 只有一个批量事务 
 
    a 过程
        1 主机发送读入数据 token ： sop sync pid(in) addr endp crc eop 
        2 设备发送数据给主机 data： sop sync pid(Data1)       crc eop ；  返回一个 空的 data， len=0 长度为0 的数据包表示 已经完成； 
        3 回应 sop sync pid(ACK) crc eop ； 如果没有发送给 主机 数据包 二十发送给主机 pid = nak 的 回应包，那么表示 失败了

    b 状态过程要与数据过程相反， 数据过程是 out 那么状态 过程就是 in，  因为。状态过程是反应 整个数据过程是否 成功；  在 host->device out 的过程 中，最终 状态 是要 device 告诉host。是否 发送 成功，但是 在 状态过程的阶段 首先是由 host 发送token in 给 device， deive 回应 数据 和 ack


# 描述符
一个 usb设备可以 多种配置， 4G上网卡可能是 u盘 也可以 配置成上网卡，但是 同一时间只有一种 配置


一个配置下 有多个接口 interface、接库等同于功能 function、
 一个接口可能有 多个设置；
 一个接口 有一个或者多个端点 endpoint 组成，端点0 属于整个设备的，端点 0 是双向的，接口还可以有气态端点，这些端点是单向的，要么是批量（Bulk）端点，要么是中断（interrupt） 端点， 同步（isochronous）端点； 端点事数据传输的 源 或者是 目的地


 EG：
    usb 耳机
    两个功能（两个接口 interface 逻辑上的 功能 ， 多个接口可以同时工作）
    1 声音

    2 按键

配置
1 对于声音 可以 播放 可以录音，也就是 主机传给 设备 或者 设备传给主机
所以在耳机上有几个端点，端点是数据传输的单位， 要跟哪一个设备 传输数据 要引入端点 
每一个设备有个 端点 0，这个是双向的可以读写， 端点0 是用来配置设备的，所以会往端点0 写入配置文件，配置成其他状态； 其他的端点不是 双向的，只能读或者只能写，所以端点0 是用来配置设备，其他的端点用来传输数据


# 描述符结构
设备描述符:{
    设备信息 id，
    端点0 可以传输多大的数据

    配置描述符:{
        有多少个接口，
        供电方式，
        最大电流
        接口描述符:{

            端点描述符：{
                端点号，端点方向，
                类型：批量、中断、同步
            },
            端点描述符
        }
    },
    配置描述符:{
        接口描述符:{
            端点描述符
        }
    }
}



描述符里有标准化的数据
bLength：描述符长度，以字节为单位。
bDescriptorType：描述符类型，用于标识描述符的类型。
bDescriptorSubtype：描述符子类型，用于标识描述符的子类型。
bMaxPacketSize0：端点0的最大包大小，以字节为单位。
bInterval：端点0的传输间隔，以帧为单位。
bNumEndpoints：端点数量，不包括端点0。
bInterfaceNumber：接口编号。
bAlternateSetting：备用设置编号。
bNumConfigurations：配置数量。
bConfigurationValue：配置值。
bInterfaceClass：接口类。
bInterfaceSubClass：接口子类。
bInterfaceProtocol：接口协议。
bEndpointAddress：端点地址。
bEndpointSize：端点最大包大小，以字节为单位。
bInterval：端点传输间隔，以帧为单位。
bRefresh：端点刷新，用于高速端点。
bSynchAddress：端点同步地址，用于高速端点。





# 枚举过程
 发送数据的时候
 data 中的每一位 对应一个 功能
比如说要获取描述符 那么 data

00 05 01 00 00 00 00 00

