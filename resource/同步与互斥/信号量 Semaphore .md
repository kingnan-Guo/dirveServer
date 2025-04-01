2025/04/01 19:10
# 信号量 Semaphore nlock 的实现
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义

信号量 
Semaphore 的结构体
```C

sources/linux-rpi-6.6.y/include/linux/semaphore.h


struct semaphore {
	raw_spinlock_t		lock;       // 信号量的实现需要借助 spinlock
	unsigned int		count;      // 计数器
	struct list_head	wait_list;  // 等待队列
};



```

    初始化 信号量 semaphore_init 后就可以使用 down 函数获得信号量，使用 up 函数释放信号量。

    1、down     sources/linux-rpi-6.6.y/kernel/locking/semaphore.c      void __sched down(struct semaphore *sem)
        a、如果之前没人锁住 先锁住信号量，如果 count > 0，则 可以获得信号量 count--，函数返回。否则 获取不到信号量，将当前进程加入等待队列，修改当前进程状态为 非 RUNNING ， 释放 spinlock ， 主动调度 schedule_timeiout()，让cpu 去处理其他进程。  如果他 被唤醒后 会去 获得信号量
        b、在读取、修改 count 时，要使用 spinlock 来实现互斥
        c、休眠时，要把进程放在 wait_list 链表中， 别的 进程释放信号量时 去 wait_list 把进程取出、唤醒；
    2、up       sources/linux-rpi-6.6.y/kernel/locking/semaphore.c      void __sched up(struct semaphore *sem)
        a、如果之前没人锁住 先锁住信号量，如果 count < 0，则 释放信号量，count++，
        b、如果 wait_list 有数据的话，唤醒 wait_list 中的第一个进程。
        c、如果 wait_list 没有数据 ，那么 只对 count++，函数返回。

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

