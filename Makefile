# # 表示编译的模块目标，obj-m 是 Kbuild 约定的写法
# obj-m := devMain.o

# # 内核构建目录
# KDIR := /lib/modules/$(shell uname -r)/build

# # 当前目录
# PWD := $(shell pwd)

# # 默认目标：编译模块
# all:
# 	$(MAKE) -C $(KDIR) M=$(PWD) modules
# 	$(CROSS_COMPILE)gcc -o main main.c

# # 清理生成的文件
# clean:
# 	$(MAKE) -C $(KDIR) M=$(PWD) clean





KERNEL_DIR := /opt/sources/linux-rpi-6.6.y
KERNEL_INCLUDE_DIR := $(KERNEL_DIR)/include
KERNEL_ARCH_INCLUDE_DIR := $(KERNEL_DIR)/arch/arm64/include

# 设置编译器标志
CFLAGS += -nostdinc
CFLAGS += -I$(KERNEL_INCLUDE_DIR)
CFLAGS += -I$(KERNEL_ARCH_INCLUDE_DIR)

# 编译目标
obj-m := my_led.o

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





