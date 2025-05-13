2025/05/11 00:10
# 深入interrupt
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义




gic->domain
gic里面创建一个 域
domain里面会有 对应关系



gic_irq_domain_hierarch_ops 中的 translate 函数
tanslate  会 解析设备树，可以解析出 hardWare_irq 硬件的 irq 和 flag 是





#  申请中断的过程
1 在设备树里 声明 要用什么中断
2 用 dmmain 来解析 设备树 DTS 
    a 解析出 硬件 中断号 hardWare_irq
    b 解析出 硬件 中断标志 flag


3 为hardWare_irq 找到空闲的 虚拟中断号 virutal_irq
4 记录下来 hardWare_irq 和 virutal_irq 的关系，记录在 gic->domain 里面
5 使用 gic_irq_domain_hierarch_ops -> alloc ,也就是 domain->alloc 来 做 映射
    a 设置 irq_desc ，irq_desc 里面有
        1 handle 指向
        2 iq_data-> irq_chip 

    所以 domain 里面会 解析 DTS ，保存 hardWare_irq 和 virutal_irq 的关系， 设置irq_desc


        



    假设解析 出 200 号中断，flag 是 0x1
        1 要为 200 号中断找到 irq_desc 
        2 在 irq_desc 数组中 找到空的一项 比如说 99 
        3 把 200 号硬件中断 和 99 号虚拟中断 关联起来
        4 会把 200 和 99  保存在 domain 里面
        5 使用 gic_irq_domain_hierarch_ops 里的 alloc 做一些设置





# 如何在设备树里申请中断
{
    interrupts = <GIC_SPI 89 IRQ_TYPE_LEVEL_HIGH>;// GIC_SPI 是 宏定义  ， 硬件中断号 89 ，中断标志是 IRQ_TYPE_LEVEL_HIGH
    interrupt-parent = <&gic>;// 中断控制器是 gic
}








# 假设 解析  注册好一个中断， 产生一个中断
cpu 就会到 异常向量表 里面去 找中断处理函数， 调用 gic_handle_irq
1 读取 gic 寄存器 里， 判断发生的 是 哪一个硬件中断， 读取寄存器， 
2 然后在domain 域里面找到硬件中断号对应的 虚拟中断号
3 然后调用 irq_desc 里面的 handle 函数，也就是中断处理函数
    从 irq_chip 里面的到 chip， chip里面有  Mask 和 unmask 函数， 也就是屏蔽和解除屏蔽中断函数，clear 函数，也就是清除中断函数， call irq action 函数，也就是调用中断处理函数
     在这个过程中 
        1 会调用 mask 屏蔽中断
        2 调用 call irq action 调用中断处理函数， 进行 中断函数的调用， 处理中断，  把 irq_desc 里面的每一个action 取出来 调用 里面的handler 函数， handler 就是由 用户传进来的函数， 处理完中断后 
        3 调用 clear 清除中断

cpu 就会调用 irq_domain




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


root@raspberrypi:/home/kingnan# cat /proc/interrupts 
           CPU0       CPU1       CPU2       CPU3       
  2:     107189     110558     113604     113174  ARMCTRL-level   1 Edge      3f00b880.mailbox
  3:         18         18         14         21  ARMCTRL-level   2 Edge      VCHIQ doorbell
 51:  792383976  793672817  791913292  792162162  ARMCTRL-level  41 Edge      dwc_otg_sim-fiq
 52:          0          0          0          0  ARMCTRL-level  42 Edge      vc4
 53:          0          0          0          0  ARMCTRL-level  43 Edge      3f004000.txp
 60:      29640      29651      29667      29713  ARMCTRL-level  50 Edge      DMA IRQ
 62:      51926      52114      51816      50851  ARMCTRL-level  52 Edge      DMA IRQ
 63:          0          0          0          0  ARMCTRL-level  53 Edge      DMA IRQ
 71:         10          8          9          8  ARMCTRL-level  61 Edge      ttyS0
 74:   98197172  100443173   99321480  101939170  ARMCTRL-level  64 Edge      dwc_otg, dwc_otg_pcd, dwc_otg_hcd:usb1
 75:          0          0          1          0  ARMCTRL-level  65 Edge      vc4 hvs
 82:          0          0          0          0  ARMCTRL-level  72 Edge      vc4 hdmi cec
 84:          0          0          0          0  ARMCTRL-level  74 Edge      vc4 crtc
 87:          0          0          0          0  ARMCTRL-level  77 Edge      vc4 crtc
 88:          0          0          0          0  ARMCTRL-level  78 Edge      vc4 crtc
 95:          0          0          0          0  ARMCTRL-level  85 Edge      3f805000.i2c
 98:      49431      50092      50171      49729  ARMCTRL-level  88 Edge      mmc0
 99:       2005       2014       2064       2047  ARMCTRL-level  89 Edge      uart-pl011
104:    1733946    1740265    1739472    1738600  ARMCTRL-level  94 Edge      mmc1
180:   14600180   23629592   16062972   14829076  bcm2836-timer   1 Edge      arch_timer
181:          0          0          0          0  bcm2836-timer   3 Edge      kvm guest vtimer
183:          0          0          0          0  bcm2836-pmu   9 Edge      arm-pmu
184:          0          0          0          0  lan78xx-irqs  17 Edge      usb-001:006:01
185:          7         11         15          9  pinctrl-bcm2835  25 Edge      enter_button
IPI0:      5598       9670       9844       9784       Rescheduling interrupts
IPI1:   9309097    3482670    7519637    9247127       Function call interrupts
IPI2:         0          0          0          0       CPU stop interrupts
IPI3:         0          0          0          0       CPU stop (for crash dump) interrupts
IPI4:         0          0          0          0       Timer broadcast interrupts
IPI5:    392008    1720717     438997     442554       IRQ work interrupts
IPI6:         0          0          0          0       CPU wake-up interrupts
Err:          0

visual_irq   CPU0上中断次数     CPU1上中断次数      CPU2上中断次数      CPU3上中断次数      中断控制器（GIC）           硬件irq    中断标志  中断控制器地址


180:        14600180            23629592            16062972        14829076            bcm2836-timer           1  Edge 是       arch_timer（用户 request_irq 传入的名字）

bcm2836-timer 是 中断控制器名字
1 是 硬件中断号
Edge 是 中断标志
arch_timer 是 用户 request_irq 传入的名字




root@raspberrypi:/home/kingnan# cd /sys/kernel/irq/
root@raspberrypi:/sys/kernel/irq# ls
1    102  106  11   113  117  120  124  128  131  135  139  142  146  15   153  157  160  164  168  171  175  179  182  19  22  26  3   33  37  40  44  48  51  55  59  62  66  7   73  77  80  84  88  91  95  99
10   103  107  110  114  118  121  125  129  132  136  14   143  147  150  154  158  161  165  169  172  176  18   183  2   23  27  30  34  38  41  45  49  52  56  6   63  67  70  74  78  81  85  89  92  96
100  104  108  111  115  119  122  126  13   133  137  140  144  148  151  155  159  162  166  17   173  177  180  184  20  24  28  31  35  39  42  46  5   53  57  60  64  68  71  75  79  82  86  9   93  97
101  105  109  112  116  12   123  127  130  134  138  141  145  149  152  156  16   163  167  170  174  178  181  185  21  25  29  32  36  4   43  47  50  54  58  61  65  69  72  76  8   83  87  90  94  98
root@raspberrypi:/sys/kernel/irq# cd 180
root@raspberrypi:/sys/kernel/irq/180# ls
actions  chip_name  hwirq  name  per_cpu_count  type  wakeup
root@raspberrypi:/sys/kernel/irq/180# cat name 
root@raspberrypi:/sys/kernel/irq/180# cat chip_name // 中断控制器名字
bcm2836-timer
root@raspberrypi:/sys/kernel/irq/180# cat hwirq // 硬件中断号
1
root@raspberrypi:/sys/kernel/irq/180# cat per_cpu_count // CPU0上中断次数     CPU1上中断次数      CPU2上中断次数      CPU3上中断次数
14624666,23671536,16091969,14872612
root@raspberrypi:/sys/kernel/irq/180# cat actions // 中断标志
arch_timer
root@raspberrypi:/sys/kernel/irq/180# cat type // 
edge