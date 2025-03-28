KERNEL_DIR := /opt/sources/linux-rpi-6.6.y
KERNEL_INCLUDE_DIR := $(KERNEL_DIR)/include
KERNEL_ARCH_INCLUDE_DIR := $(KERNEL_DIR)/arch/arm64/include

# 设置编译器标志 CFLAGS（Compiler Flags）是 传递给 C 语言编译器 gcc 的编译选项。
# CFLAGS += -nostdinc
# CFLAGS += -I$(KERNEL_INCLUDE_DIR)
# CFLAGS += -I$(KERNEL_ARCH_INCLUDE_DIR)
# # 只用于内核模块的 CFLAGS
KERNEL_CFLAGS := -nostdinc -I$(KERNEL_INCLUDE_DIR) -I$(KERNEL_ARCH_INCLUDE_DIR)



# 指定交叉编译工具链
CROSS_COMPILE := aarch64-linux-gnu-
# 模块地址
MODULES_DIR := dirverModules
APP_MODULES_DIR := modules

# 动态获取 modules 下所有子目录作为头文件路径
APP_SUBDIRS := $(shell find $(APP_MODULES_DIR) -type d)
USER_CFLAGS := $(foreach dir,$(APP_SUBDIRS),-I$(PWD)/$(dir))

# 收集源文件
SOURCES := main.c
MODULE_SOURCES := $(wildcard $(APP_MODULES_DIR)/*/*.c)



# obj-m := devMain.o







# # my_board 的 编译  ---------------------
# my_device-y := my_drv.o my_board.o
# obj-m += my_device.o

# my_device-y := $(MODULES_DIR)/my_board/my_drv.o $(MODULES_DIR)/my_board/my_board.o
# obj-m += my_device.o







# # # my_chip_device 的 编译 ---------------------
# my_chip_device-y := my_drv.o my_chip_board_gpio.o my_board_n.o

# my_chip_device-y := $(MODULES_DIR)/my_chip_device/my_drv.o $(MODULES_DIR)/my_chip_device/my_chip_board_gpio.o $(MODULES_DIR)/my_chip_device/my_board_n.o
# obj-m += my_chip_device.o





# my_platform_device_gpio 总线设备驱动模型  的 编译  出三个 ko 程序---------------------
# obj-m += my_drv.o my_chip_board_gpio.o my_board_n.o

# my_drv-y := $(MODULES_DIR)/my_platform_device_gpio/my_drv.o
# my_chip_board_gpio-y := $(MODULES_DIR)/my_platform_device_gpio/my_chip_board_gpio.o
# my_board_n-y := $(MODULES_DIR)/my_platform_device_gpio/my_board_n.o
# obj-m += my_drv.o
# obj-m += my_chip_board_gpio.o
# obj-m += my_board_n.o



# my_platform_device_dtb_button 
# 有 dtb 
# obj-m += my_drv.o my_chip_board_button.o

# my_chip_board_button-y := $(MODULES_DIR)/my_platform_device_dtb_button/my_chip_board_button.o
# my_drv-y := $(MODULES_DIR)/my_platform_device_dtb_button/my_drv.o
# obj-m += my_chip_board_button.o
# obj-m += my_drv.o




# obj-m := my_drv.o


# # my_interrupt 中断 的 编译  ---------------------
# interrupt_sleep_wake_up_circle-y := $(MODULES_DIR)/interrupt_sleep_wake_up_circle/interrupt_sleep_wake_up_circle.o
# obj-m := interrupt_sleep_wake_up_circle.o



# poll ---------------------
# interrupt_sleep_wake_up_poll-y := $(MODULES_DIR)/interrupt_sleep_wake_up_poll/interrupt_sleep_wake_up_poll.o
# obj-m := interrupt_sleep_wake_up_poll.o


# fasync ---------------------
# interrupt_sleep_wake_up_fasync-y := $(MODULES_DIR)/interrupt_sleep_wake_up_fasync/interrupt_sleep_wake_up_fasync.o
# obj-m := interrupt_sleep_wake_up_fasync.o


# noblock
# interrupt_sleep_wake_up_noblock-y := $(MODULES_DIR)/interrupt_sleep_wake_up_noblock/interrupt_sleep_wake_up_noblock.o
# obj-m := interrupt_sleep_wake_up_noblock.o

# timer
interrupt_sleep_wake_up_timer-y := $(MODULES_DIR)/interrupt_sleep_wake_up_timer/interrupt_sleep_wake_up_timer.o
obj-m := interrupt_sleep_wake_up_timer.o


all:
	# 进入内核源码目录并构建模块
	# make -C $(KERNEL_DIR) M=$(PWD) modules
	# make -C $(KERNEL_DIR) M=$(PWD) modules CROSS_COMPILE=$(CROSS_COMPILE) CC=$(CROSS_COMPILE)gcc
	# CFLAGS="$(KERNEL_CFLAGS)" 的目的是 传递给 C 语言编译器 gcc 的编译选项, 只在内核生效
	make -C $(KERNEL_DIR) M=$(PWD) modules CROSS_COMPILE=$(CROSS_COMPILE) CC=$(CROSS_COMPILE)gcc CFLAGS="$(KERNEL_CFLAGS)"

	# 交叉编译 应用层 程序
	# $(CROSS_COMPILE)gcc -o main main.c
	# aarch64-linux-gnu-gcc -o main main.c
	$(CROSS_COMPILE)gcc $(USER_CFLAGS) -o main $(SOURCES) $(MODULE_SOURCES)

clean:
	# 清理
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf modules.order
	rm -rf main


.PHONY: all clean














# KERNEL_DIR := /opt/sources/linux-rpi-6.6.y
# KERNEL_INCLUDE_DIR := $(KERNEL_DIR)/include
# KERNEL_ARCH_INCLUDE_DIR := $(KERNEL_DIR)/arch/arm64/include

# # 指定交叉编译工具链
# CROSS_COMPILE := aarch64-linux-gnu-

# # 模块地址
# MODULES_DIR := dirverModules
# APP_MODULES_DIR := modules

# # 获取应用层的头文件路径
# APP_SUBDIRS := $(shell find $(APP_MODULES_DIR) -type d)
# USER_CFLAGS := $(foreach dir,$(APP_SUBDIRS),-I$(PWD)/$(dir))

# # 只用于内核模块的 CFLAGS
# KERNEL_CFLAGS := -nostdinc -I$(KERNEL_INCLUDE_DIR) -I$(KERNEL_ARCH_INCLUDE_DIR)

# # 收集源文件
# SOURCES := main.c
# MODULE_SOURCES := $(wildcard $(APP_MODULES_DIR)/*/*.c)

# # fasync
# interrupt_sleep_wake_up_fasync-y := $(MODULES_DIR)/interrupt_sleep_wake_up_fasync/interrupt_sleep_wake_up_fasync.o
# obj-m := interrupt_sleep_wake_up_fasync.o

# all:
# 	# 进入内核源码目录并构建模块
# 	make -C $(KERNEL_DIR) M=$(PWD) modules CROSS_COMPILE=$(CROSS_COMPILE) CC=$(CROSS_COMPILE)gcc CFLAGS="$(KERNEL_CFLAGS)"

# 	# 编译应用层程序（不加 -nostdinc）
# 	$(CROSS_COMPILE)gcc $(USER_CFLAGS) -o main $(SOURCES) $(MODULE_SOURCES)

# clean:
# 	# 清理
# 	make -C $(KERNEL_DIR) M=$(PWD) clean
# 	rm -rf modules.order
# 	rm -rf main

# .PHONY: all clean
