


# 运行顺序 =================
insmod my_drv.ko
insmod my_board_n.ko

insmod my_chip_board_gpio.ko


cat /proc/devices  


ls -l /dev/my_board_device_*
chmod +x main


dmesg | tail

./main /dev/my_board_device_0 on
./main /dev/my_board_device_1 off

<!-- rmmod my_driver.ko -->



rmmod my_chip_board_gpio.ko
rmmod my_board_n.ko
rmmod my_drv.ko 

失败报错  无法 卸载驱动






# 直接操作 gpio =============

root@raspberrypi:/opt# cd /sys/class/gpio
root@raspberrypi:/sys/class/gpio# ls
export  gpiochip512  gpiochip566  unexport

root@raspberrypi:/sys/class/gpio# echo 529 > /sys/class/gpio/export 
root@raspberrypi:/sys/class/gpio# echo 529 > /sys/class/gpio/unexport 




root@raspberrypi:/sys/class/gpio# usermod -aG gpio root

gpioget gpiochip0 27  # 获取 GPIO 27 的当前值
root@raspberrypi:/sys/class/gpio# gpioset gpiochip0 27=1
root@raspberrypi:/sys/class/gpio# gpioset gpiochip0 27=0


root@raspberrypi:/opt/TEMP# echo 1 | sudo tee /dev/my_led
root@raspberrypi:/opt/TEMP# echo 0 | sudo tee /dev/my_led


cat /sys/class/gpio/gpio27/value








# ==================== 编译启动 驱动模块 ===================================

# 首先未来要 手写 Makefile ； cmake 无法再 驱动中使用

# 编译驱动模块
make

ls -l devMain.ko 
# 装载 驱动模块
sudo insmod devMain.ko
sudo insmod my_led.ko
sudo insmod kingnan_led.ko

# 查看设备号
cat /proc/devices 

# 查看驱动模块是否加载成功
dmesg | tail

# 查看驱动模块是否加载成功
cat /proc/devices
    打印 ： 241 my_test_device

# 创建设备节点
sudo mknod /dev/mydev c 241 0

# 查看是否创建成功
ls -l /dev/my_device 
ls -l /dev/my_led 
ls -l /dev/my_device_0
# 测试驱动模块
./main /dev/mydev hello world


chmod +x main

./main /dev/my_led on

# 查看设备节点是否创建成功
dmesg | tail

# 删除设备节点
sudo rm /dev/mydev
sudo rm /dev/my_led

# 删除驱动模块
sudo rmmod devMain
rmmod my_led

# 查看驱动模块是否加载成功 打印中找不到  241 my_test_device
cat /proc/devices 

# 进入 系统目录 查看驱动模块
cd /sys/class 
    这里就有 my_test_device

cd /sys/class/my_test_device

ll
    lrwxrwxrwx 1 root root 0 Jan 13 22:35 my_test_device -> ../../devices/virtual/my_test_device_class/my_test_device

cd ../../devices/virtual/my_test_device_class/my_test_device

cat dev
    241:0
    根据 主次设备号 生成设备节点



# 打开内核 打印信息
echo "7 4 1 7" > /proc/sys/kernel/printk

# 关闭内核 打印信息
echo "4 4 1 7" > /proc/sys/kernel/printk

# 查看内核 打印信息
cat /proc/sys/kernel/printk

# 打印信息
dmesg | tail


# 查看使用函数 的 命令
grep -rn "register_chrdev" *



