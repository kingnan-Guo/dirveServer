KERNEL_DIR := /opt/sources/linux-rpi-6.6.y
KERNEL_INCLUDE_DIR := $(KERNEL_DIR)/include
KERNEL_ARCH_INCLUDE_DIR := $(KERNEL_DIR)/arch/arm64/include

# 设置编译器标志
CFLAGS += -nostdinc
CFLAGS += -I$(KERNEL_INCLUDE_DIR)
CFLAGS += -I$(KERNEL_ARCH_INCLUDE_DIR)


# 指定交叉编译工具链
CROSS_COMPILE := aarch64-linux-gnu-
# 模块地址
MODULES_DIR := dirverModules


# # my_board 的 编译  ---------------------
# my_device-y := my_drv.o my_board.o
# obj-m += my_device.o

# my_device-y := $(MODULES_DIR)/my_board/my_drv.o $(MODULES_DIR)/my_board/my_board.o
# obj-m += my_device.o







# # # my_chip_device 的 编译 ---------------------
# my_chip_device-y := my_drv.o my_chip_board_gpio.o my_board_n.o

# my_chip_device-y := $(MODULES_DIR)/my_chip_device/my_drv.o $(MODULES_DIR)/my_chip_device/my_chip_board_gpio.o $(MODULES_DIR)/my_chip_device/my_board_n.o
# obj-m += my_chip_device.o





# my_platform 总线设备驱动模型  的 编译  出三个 ko 程序---------------------
# obj-m += my_drv.o my_chip_board_gpio.o my_board_n.o

# my_drv-y := $(MODULES_DIR)/my_platform/my_drv.o
# my_chip_board_gpio-y := $(MODULES_DIR)/my_platform/my_chip_board_gpio.o
# my_board_n-y := $(MODULES_DIR)/my_platform/my_board_n.o
# obj-m += my_drv.o
# obj-m += my_chip_board_gpio.o
# obj-m += my_board_n.o





# obj-m := my_drv.o


# # my_interrupt 中断 的 编译  ---------------------
my_interrupt-y := $(MODULES_DIR)/interrupt/my_interrupt.o
obj-m := my_interrupt.o



all:
	# 进入内核源码目录并构建模块
	make -C $(KERNEL_DIR) M=$(PWD) modules
	# 交叉编译
	# $(CROSS_COMPILE)gcc -o main main.c
	aarch64-linux-gnu-gcc -o main main.c
clean:
	# 清理
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf modules.order
	rm -rf main





