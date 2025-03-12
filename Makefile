KERNEL_DIR := /opt/sources/linux-rpi-6.6.y
KERNEL_INCLUDE_DIR := $(KERNEL_DIR)/include
KERNEL_ARCH_INCLUDE_DIR := $(KERNEL_DIR)/arch/arm64/include

# 设置编译器标志
CFLAGS += -nostdinc
CFLAGS += -I$(KERNEL_INCLUDE_DIR)
CFLAGS += -I$(KERNEL_ARCH_INCLUDE_DIR)

# 编译目标
my_device-y := my_drv.o my_board.o
# obj-m := my_drv.o
obj-m += my_device.o


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





