2025/04/01 19:30
# 互斥量 mutex
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义
互斥量 mutex

```C
struct mutex {
	atomic_long_t		owner;      // 持有者
	raw_spinlock_t		wait_lock;  // 自旋锁
#ifdef CONFIG_MUTEX_SPIN_ON_OWNER
	struct optimistic_spin_queue osq; /* Spinner MCS lock */
#endif
	struct list_head	wait_list;  // 等待队列
#ifdef CONFIG_DEBUG_MUTEXES
	void			*magic;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map	dep_map;
#endif
};

```

    mutex 结构体
        1、mutex 的实现要 使用  spinlock
        2、ower 只适用于互斥量，用于表示哪个进程拥有这个互斥量， owner 是一个原子变量，用于记录当前拥有互斥量的进程的 PID， 并不时谁拥 谁释放；只要 用于 调试 优化的
        3、如果 cpu0 先获得 mutext_lock() 这时 有cpu1 来 去调这个，发现被 cpu1 占用，那么 cpu1 就会 死等 一会（spin on ower）， 要实现 spin 功能 必须要有 ower，否则 cpu1 不知道谁占用了，就不知道该死等谁


    初始化
        mutex_init(&mutex);
    上锁
        mutex_lock(&mutex);
    解锁
        mutex_unlock(&mutex);
    销毁
        mutex_destroy(&mutex);



mutex_lock 函数的实现

sources/linux-rpi-6.6.y/kernel/locking/mutex.c
```C

    void __sched mutex_lock(struct mutex *lock)
    {
        might_sleep();

        if (!__mutex_trylock_fast(lock))// 尝试快速上锁
            __mutex_lock_slowpath(lock);// 慢速上锁
    }
    EXPORT_SYMBOL(mutex_lock);

```


    1、fastpath
        mutex 比 semaphore 复制，但效率高
        如果 fastpath 成功 ，那么 slowpath 就不会使用

        mutex 的 count  1表示 unlock 0表示 locked，负数 表示 locked ，并且有其他程序在等待；



        [1、unlock 直接获得锁, 2、无人等待 直接 释放]

        如果变量 减 1 后, 如果 为 0 ，  所以直接获得锁，如果为 减 1 为  小于 0，则进入 slowpath
        如果 变量 加 1 后， 如果 为 0 那么说明 有人等待， 之前 小于0，则进入 slowpath； 如果 加一 后 大于0，则直接 释放锁 ，不用去唤醒

        fastpath 快的原因是 不需要 等待 ，直接获得锁。释放锁的时候也不需要去唤醒 其他等待的 队列

    2、slowpath

        [1、locked 的时候，会进入等待队列，并且进入睡眠状态，等待被唤醒 , 2、有人等待 wake  up]

在执行过程
    1、 unlock 直接获得锁
    2、 locked 的时候，会进入等待队列，并且进入睡眠状态，等待被唤醒



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

