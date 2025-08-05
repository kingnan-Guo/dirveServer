
2025/08/04 00:10
#  usb设备驱动模型
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义

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
