# 表示编译的模块目标，obj-m 是 Kbuild 约定的写法
obj-m := devMain.o

# 内核构建目录
# KDIR := /lib/modules/$(shell uname -r)/build
# /usr/src/kernels/3.10.0-1160.119.1.el7.x86_64/
KDIR := /usr/src/kernels/3.10.0-1160.119.1.el7.x86_64/

# 当前目录
PWD := $(shell pwd)

# 默认目标：编译模块
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	$(CROSS_COMPILE)gcc -o main main.c

# 清理生成的文件
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
