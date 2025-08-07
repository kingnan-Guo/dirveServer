
2025/08/04 00:10
#  usb设备驱动模型
    分支
        dirver_raspberry_usb_mouse_dev_v0.5.3

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/usb_mouse_dev/usb_mouse_dev.c


# 这个是 鼠标设备驱动 作为 参考
[text](usbmouse.c)


# 定义
usb_mouse_dev
 根据总线设备驱动模型来做



struct bus_type usb_bus_type {
    .name = "usb",
    .match = usb_device_match,
    .uevent = usb_uevent,
};


#  usb_device 结构体
struct usb_device {
    struct usb_bus       *bus;       /* the bus this device is on */
    struct usb_dev       *dev;       /* generic device info */
    struct usb_device    *parent;    /* parent usb device */
    struct usb_host_endpoint *ep0;    /* endpoint 0 */
}



#  总线右侧是 usb_interface 结构体
struct usb_interface {
    struct usb_device *dev; /* the device this interface belongs to */
    struct usb_host_endpoint *ep_in[USB_MAXENDPOINTS]; /* endpoints for this interface */

}


# 总线左侧 是 usb_dirver
struct usb_driver {
    struct device_driver driver;
    const struct usb_device_id *id_table;
    int (*probe) (struct usb_interface *intf, const struct usb_device_id *id);
    void (*disconnect) (struct usb_interface *intf);
    int (*ioctl) (struct usb_interface *intf, unsigned int code, void *buf);
    int (*suspend) (struct usb_interface *intf, pm_message_t message);
    int (*resume) (struct usb_interface *intf);
    int (*pre_reset) (struct usb_interface *intf);
}

左右两边 对上后 执行 usb_driver 的 probe 函数


# 流程


# 执行顺序


# 内部机制


# Makefile
# # usb_mouse_dev 驱动 -------------------
usb_mouse_dev-y := $(MODULES_DIR)/usb_mouse_dev/usb_mouse_dev.o
obj-m += usb_mouse_dev.o


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
# 扩展




# 普通buffer

在正常操作

我们申请 buffer， 然后 使用usb_device_dirver 接口，把 buffer  通过 usb_controller( usb 控制器 硬件) 传给 usb_device
这时候 申请的 buffer 是 virtual address  ， 连续的。虚拟地址 对应一个 物理上 不连续 的 硬件内存地址

所以在 发送的时候 ， usb_controller( usb 控制器 硬件) 是没办法 处理 物理上 不连续 的 硬件内存地址 ， 所以在传输之前 就要临时分配一个 物理上连续的临时 的 TEMP buffer， 把 申请的 buffer 中的数据 复制到 TEMP buffer 中， 然后把 TEMP buffer 的 物理地址 传给 usb_controller( usb 控制器 硬件)

在读取数据也是一样的 ， usb_controller( usb 控制器 硬件) 读取数据后 ， 把数据 复制到 TEMP buffer 中， 然后把 TEMP buffer 中的数据 复制到 物理上 不连续 的 硬件内存地址 buffer 中， 然后在由 virtual address 与 物理上 不连续 的 硬件内存地址对应 ， 最后 传给 用户空间


# DMA_buffer
DMA_buffer 是 直接连续的  物理内存， 所以就省去了 复制到 TEMP Buffer 的 过程

void *usb_alloc_coherents(struct usb_device *dev, unsigned long size, gfp_t mem_flags, dma_addr_t *dma_handle)

物理地址保存在 dma_handle 中， 虚拟地址保存在 返回值中
usb->transfer_dma = dma_handle;
usb->transfer_flags |= URB_NO_TRANSFER_DMA;// 高速 usb 已经有物理地址了，不需要再 分配 TEMP buffer 了





# 可以使用 input 系统的框架编写 usb_dirver

现在需要再 usb_dirver 的 probe 函数中 构造注册 input_dev

为了 获取数据需要
1 构造 提交 urb
2 urb 的回调函数 里，向 input系统上报数据




sources/linux-rpi-6.6.y/drivers/usb/core/usb.h
sources/linux-rpi-6.6.y/drivers/usb/core/usb.c

sources/linux-rpi-6.6.y/include/linux/usb.h



依赖
sources/linux-rpi-6.6.y/include/linux/usb.h





ls /dev/input/event*
hexdump /dev/input/event











# ======

root@raspberrypi:/dev# cd /sys/bus/usb/
root@raspberrypi:/sys/bus/usb# ls
devices  drivers  drivers_autoprobe  drivers_probe  uevent
root@raspberrypi:/sys/bus/usb# cd devices/

#  没有插入 鼠标
root@raspberrypi:/sys/bus/usb/devices# ls              
1-0:1.0  1-1  1-1.1  1-1:1.0  1-1.1.1  1-1.1:1.0  1-1.1.1:1.0  usb1

# 插入 鼠标
root@raspberrypi:/sys/bus/usb/devices# ls                
1-0:1.0  1-1  1-1.1  1-1:1.0  1-1.1.1  1-1.1:1.0  1-1.1.1:1.0  1-1.1.2  1-1.1.2:1.0  usb1

# 查看 1-1.1.2 
root@raspberrypi:/sys/bus/usb/devices# cd 1-1.1.2
1-1.1.2/     1-1.1.2:1.0/ 
root@raspberrypi:/sys/bus/usb/devices# cd 1-1.1.2/

root@raspberrypi:/sys/bus/usb/devices/1-1.1.2# ls
1-1.1.2:1.0        bcdDevice            bDeviceProtocol  bMaxPacketSize0     bNumInterfaces  descriptors  devpath  ep_00      ltm_capable   port     quirks     rx_lanes   tx_lanes  version
authorized         bConfigurationValue  bDeviceSubClass  bMaxPower           busnum          dev          devspec  idProduct  manufacturer  power    removable  speed      uevent
avoid_reset_quirk  bDeviceClass         bmAttributes     bNumConfigurations  configuration   devnum       driver   idVendor   maxchild      product  remove     subsystem  urbnum
root@raspberrypi:/sys/bus/usb/devices/1-1.1.2# ls -l
total 0
drwxr-xr-x 5 root root     0 Aug  6 18:02 1-1.1.2:1.0
-rw-r--r-- 1 root root  4096 Aug  6 18:02 authorized
-rw-r--r-- 1 root root  4096 Aug  6 18:02 avoid_reset_quirk
-r--r--r-- 1 root root  4096 Aug  6 18:02 bcdDevice
-rw-r--r-- 1 root root  4096 Aug  6 18:02 bConfigurationValue
-r--r--r-- 1 root root  4096 Aug  6 18:02 bDeviceClass
-r--r--r-- 1 root root  4096 Aug  6 18:02 bDeviceProtocol
-r--r--r-- 1 root root  4096 Aug  6 18:02 bDeviceSubClass
-r--r--r-- 1 root root  4096 Aug  6 18:02 bmAttributes
-r--r--r-- 1 root root  4096 Aug  6 18:02 bMaxPacketSize0
-r--r--r-- 1 root root  4096 Aug  6 18:02 bMaxPower
-r--r--r-- 1 root root  4096 Aug  6 18:02 bNumConfigurations
-r--r--r-- 1 root root  4096 Aug  6 18:02 bNumInterfaces
-r--r--r-- 1 root root  4096 Aug  6 18:02 busnum
-r--r--r-- 1 root root  4096 Aug  6 18:02 configuration
-r--r--r-- 1 root root 65553 Aug  6 18:02 descriptors
-r--r--r-- 1 root root  4096 Aug  6 18:02 dev
-r--r--r-- 1 root root  4096 Aug  6 18:02 devnum
-r--r--r-- 1 root root  4096 Aug  6 18:02 devpath
-r--r--r-- 1 root root  4096 Aug  6 18:02 devspec
lrwxrwxrwx 1 root root     0 Aug  6 18:02 driver -> ../../../../../../../../bus/usb/drivers/usb   # 这里 展示 鼠标被 接管了
drwxr-xr-x 3 root root     0 Aug  6 18:02 ep_00
-r--r--r-- 1 root root  4096 Aug  6 18:02 idProduct
-r--r--r-- 1 root root  4096 Aug  6 18:02 idVendor
-r--r--r-- 1 root root  4096 Aug  6 18:02 ltm_capable
-r--r--r-- 1 root root  4096 Aug  6 18:02 manufacturer
-r--r--r-- 1 root root  4096 Aug  6 18:02 maxchild
lrwxrwxrwx 1 root root     0 Aug  6 18:02 port -> ../1-1.1:1.0/1-1.1-port2
drwxr-xr-x 2 root root     0 Aug  6 18:02 power
-r--r--r-- 1 root root  4096 Aug  6 18:02 product
-r--r--r-- 1 root root  4096 Aug  6 18:02 quirks
-r--r--r-- 1 root root  4096 Aug  6 18:02 removable
--w------- 1 root root  4096 Aug  6 18:02 remove
-r--r--r-- 1 root root  4096 Aug  6 18:02 rx_lanes
-r--r--r-- 1 root root  4096 Aug  6 18:02 speed
lrwxrwxrwx 1 root root     0 Aug  6 18:02 subsystem -> ../../../../../../../../bus/usb
-r--r--r-- 1 root root  4096 Aug  6 18:02 tx_lanes
-rw-r--r-- 1 root root  4096 Aug  6 18:02 uevent
-r--r--r-- 1 root root  4096 Aug  6 18:02 urbnum
-r--r--r-- 1 root root  4096 Aug  6 18:02 version




root@raspberrypi:/sys/bus/usb/devices# ls
1-0:1.0  1-1  1-1.1  1-1:1.0  1-1.1.1  1-1.1:1.0  1-1.1.1:1.0  1-1.1.2  1-1.1.2:1.0  usb1

# 查看  1-1.1.2:1.0 
root@raspberrypi:/sys/bus/usb/devices# cd  1-1.1.2:1.0 
root@raspberrypi:/sys/bus/usb/devices/1-1.1.2:1.0# ls
0003:17EF:608D.0006  bAlternateSetting  bInterfaceNumber    bInterfaceSubClass  driver  modalias  subsystem             uevent
authorized           bInterfaceClass    bInterfaceProtocol  bNumEndpoints       ep_81   power     supports_autosuspend
root@raspberrypi:/sys/bus/usb/devices/1-1.1.2:1.0# ls  -l
total 0
drwxr-xr-x 5 root root    0 Aug  6 18:02 0003:17EF:608D.0006
-rw-r--r-- 1 root root 4096 Aug  6 18:03 authorized
-r--r--r-- 1 root root 4096 Aug  6 18:03 bAlternateSetting
-r--r--r-- 1 root root 4096 Aug  6 18:02 bInterfaceClass
-r--r--r-- 1 root root 4096 Aug  6 18:02 bInterfaceNumber
-r--r--r-- 1 root root 4096 Aug  6 18:02 bInterfaceProtocol
-r--r--r-- 1 root root 4096 Aug  6 18:02 bInterfaceSubClass
-r--r--r-- 1 root root 4096 Aug  6 18:02 bNumEndpoints
lrwxrwxrwx 1 root root    0 Aug  6 18:02 driver -> ../../../../../../../../../bus/usb/drivers/usbhid
drwxr-xr-x 3 root root    0 Aug  6 18:03 ep_81
-r--r--r-- 1 root root 4096 Aug  6 18:03 modalias
drwxr-xr-x 2 root root    0 Aug  6 18:03 power
lrwxrwxrwx 1 root root    0 Aug  6 18:02 subsystem -> ../../../../../../../../../bus/usb
-r--r--r-- 1 root root 4096 Aug  6 18:03 supports_autosuspend
-rw-r--r-- 1 root root 4096 Aug  6 18:02 uevent
root@raspberrypi:/sys/bus/usb/devices/1-1.1.2:1.0# 

# 最终结论 是 需要卸载掉 自带的 usb 驱动



root@raspberrypi:/sys/bus/usb/devices/1-1.1.2:1.0# ls /sys/bus/usb/drivers/
brcmfmac  hub  lan78xx  r8152  r8152-cfgselector  smsc95xx  uas  usb  usbfs  usbhid  usb_mouse_as_key  usb-storage







root@raspberrypi:/home/kingnan/TEMP/usb/鼠标驱动# insmod usb_mouse_dev.ko 
root@raspberrypi:/home/kingnan/TEMP/usb/鼠标驱动# dmesg | tail


[ 1295.913399] /opt/github/dirveServer/dirverModules/usb_mouse_dev/usb_mouse_dev.c usb_mouse_as_key__init 284
[ 1295.913700] usbcore: registered new interface driver usb_mouse_as_key




root@raspberrypi:/sys/bus/usb/devices# ls
1-0:1.0  1-1  1-1.1  1-1:1.0  1-1.1.1  1-1.1:1.0  1-1.1.1:1.0  1-1.1.2  1-1.1.2:1.0  usb1
root@raspberrypi:/sys/bus/usb/devices# 
ls /sys/bus/usb/devices




watch -n 1 "dmesg"

dmesg | less -F







# 解绑 内核驱动

ls -l /sys/bus/usb/devices/1-1.1.2:1.0/driver


# 解绑 内核驱动
echo -n "1-1.1.2:1.0" > /sys/bus/usb/devices/1-1.1.2:1.0/driver/unbind
echo -n "1-1.1.2:1.0" | sudo tee /sys/bus/usb/drivers/usbhid/unbind



# 绑定 内核驱动
echo -n "usb_mouse_as_key" > /sys/bus/usb/devices/1-1.1.2:1.0/driver/bind

# 绑定 usb_mouse_dev
echo -n "1-1.1.2:1.0" | sudo tee /sys/bus/usb/drivers/usb_mouse_as_key/bind




root@raspberrypi:/sys/bus/usb/devices/1-1.1.2# ls -l /sys/bus/usb/devices/1-1.1.2:1.0/driver
lrwxrwxrwx 1 root root 0 Aug  6 18:44 /sys/bus/usb/devices/1-1.1.2:1.0/driver -> ../../../../../../../../../bus/usb/drivers/usb_mouse_as_key



