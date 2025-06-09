2025/03/29 00:10
# 名称
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



# 扩展

APP 

i2c Device Dirver

i2c core

i2c Controller Dirver



1 App  命令 给 i2c Device Dirver 层

0x50 W 0x00 abc


2 i2c Device Dirver 的操作是。 发出的信号
[
    start 0x50 0x00  a stop
    start 0x50 0x01  b stop
    start 0x50 0x02  c stop

]


3 驱动发出 的信号 到 i2c core 层， 拆分出 三个信号 ， 调用 i2c core 中断额某一个函数 ，

    i2c core 抽象出 统一的接口 ，给 上层的 抽象程序 提供 统一出 标准的接口， 让 驱动来调用，然后把数据发送给 最底层的 i2c controller driver


4 然后 来通过  i2c Controller Dirver （i2c 控制器） 把 数据 （start 0x50 0x00  a stop）发送给  i2c 设备，这里的 i2c设备 可能是  at24c02







```C

struct i2c_adapter {
    struct device *dev;
    struct i2c_algorithm *algo;// i2c 算法 重点
    struct list_head devices;
    struct list_head adapters;
    unsigned int timeout;
    unsigned int retries;
    unsigned int nr;// i2c 第几条 i2c 总线 重点
    unsigned int functional;
    struct mutex bus_lock;
    struct completion dev_released;
    struct device_address_list *dev_addr_list;
}



struct i2c_algorithm {
    int (*master_xfer)(struct i2c_adapter *adap, struct i2c_msg *msgs,
               int num);// 重点 ，传输 i2c 的消息，发起完整的 i2c 信息 传输
    int (*smbus_xfer) (struct i2c_adapter *adap, u16 addr,
               unsigned short flags, char read_write,
               u8 command, int size, union i2c_smbus_data *data);
    int (*master_xfer_atomic)(struct i2c_adapter *adap,
                  struct i2c_msg *msgs, int num);// 原子版本 
    int (*smbus_xfer_atomic)(struct i2c_adapter *adap, u16 addr,
                  unsigned short flags, char read_write,
                  u8 command, int size, union i2c_smbus_data *data);
    int (*recover_bus)(struct i2c_adapter *adap);
    u32 (*functionality)(struct i2c_adapter *adap);// 提供哪些 功能
}
```