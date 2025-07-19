2025/03/29 00:10
# spi 设备驱动程序
    分支
        dirver_raspberry_XXXXX_vX.X.X

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


make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs -j$(nproc) 


dtc -I fs /sys/firmware/devicetree/base | less
# 扩展


当 platform device 与 platform driver 配对成功之后 就会调用 probe 函数 ，就会去生成 spi master， spi master 有函数就会执行 spi 的操作，还会去 解析设备树， 解析设备树 会生成一个 spi device，

spi总线 spi bus type
spi总线 就是一个结构体， 用来管理 spi device 和 spi dirver，都遵守 总线设备 驱动这些模型

在解析设备树的时候会得到  spi device ，这些  spi device 会注册进入到 spi_bus_type 里面，这个 spi_bus_type 总线里面会会挂载 一个 或者 多个 spi device

目的：
写出驱动程序 ，注册 spi dirver， 挂载 在 spi_bus_type 链表上， 如果可以与 spi device 匹配， 就会 调用 probe 函数， 调用后 就会去 注册 字符设备

在 adc 中会使用 ioctl



spi_master 结构体 中包含

spi_transfer{
    *tx_buf
    *rx_buf
}


有数据要发送就要放到 tx_buf 里面，有数据要接收就要放到 rx_buf 里面；
tx_buf 会通过 spi_master 里面的 transfer 函数 发送出去，rx_buf 会通过 spi_master 里面的 transfer 函数 接收回来；

如果要发送多个 就要创建 多个 spi_transfer ，然后放到 spi_message 里面， spi_message 里面有一个 spi_transfer 的数组，然后 spi_master 里面的 transfer 函数 会把 spi_message 里面的 spi_transfer 依次发送出去；

spi_message{
    struct list_head transfer_list;// 这里放  spi_transfer 的链表
    struct spi_device *spi;
}

报所有 的 spi_transfer 使用 spi_message_add_tail()

spi_sync() 函数 会把 spi_message 里面的 spi_transfer 依次发送出去；是 阻塞的；同步函数

也有异步操作 spi_async() 函数