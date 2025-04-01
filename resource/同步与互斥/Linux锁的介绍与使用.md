2025/04/01 12:10
# Linux锁的介绍与使用
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义


锁的类型
    自旋锁
        1、无法获得的时候，不会休眠，会一直等待
            raw_spinlock    原始自旋锁
            bit spinlock    位自旋锁

            自旋锁的加锁，解锁函数，都是原子操作，还可以加上各种 后缀，表示 加锁解锁的时候 ，还可以做其他事
            spin_lock
            spin_unlock

 
            raw_spin_lock           禁止CPU调度
            raw_spin_unlock         开启CPU调度

            spin_lock_bh            加锁时 禁止下半部(软中断)，下半部是中断处理程序
            spin_unlock_bh          解锁时 开启下半部, 使能 软中断

            spin_lock_irq           加锁时 禁止中断
            spin_unlock_irq         解锁时 开启中断

         
            spin_lock_irqsave       加锁时 禁止中断，保存中断状态
            spin_unlock_irqrestore  解锁时 开启中断，恢复中断状态，之前是开启就是开启，之前是关闭就是关闭

        2、cpu0 、cpu1 同时访问 一段 临界资源
            a、任意时刻只能有一个 cpu 访问，这个cpu 访问 后解锁
            b、在 cpu0 加锁的时候，如果 cpu1 也想访问，cpu1 就会一直等待 （循环），直到 cpu0 解锁

        3、单 cpu 也有自旋锁
            a、单 cpu 也有自旋锁，但是没有意义，因为单 cpu 的时候，只有一个 cpu 在运行，所以不需要加锁
            b、担心被 别的 进程打断 ， 所以可以禁止 别的进程 调度 ，使用 preempt_disable() 和 preempt_enable() 来禁止和开启调度，叫做 禁止抢占

    睡眠锁

        1、无法获得的时候，会休眠，不会一直等待
            mutex                       互斥锁
            rt_mutex                    实时互斥锁

            rwlock                      读写锁
            semaphore
            completion

            mutex_lock
            mutex_unlock

            rwlock_read_lock
            rwlock_read_unlock

            rwlock_write_lock
            rwlock_write_unlock

            semaphore_down
            semaphore_up

            completion_wait
            completion_signal

            mutex_lock_interruptible
            mutex_unlock_interruptible

            mutex_lock_killable
            mutex_unlock_killable

            mutex_lock_trylock
            mutex_unlock_trylock

            mutex_lock_nested
            mutex_unlock_nested





信号量 Semaphore
    用在两个不同的个体之间 用来同步信息，传递资源
    信号量是一个非负整数，所有试图进入临界区的进程，必须先申请信号量，申请成功后，信号量值减1，若申请不成功，则进程挂起，直到信号量值大于0为止

    生产出 n 个 ，消费者可以 消费 n 个， 如果消费者消费的比生产的多，那么就会阻塞，等待生产者生产， 通过信号量来传递资源

    Semaphore 函数在 /include/linux/semaphore.h 中定义
```C
    struct semaphore {
        raw_spinlock_t		lock;
        unsigned int		count;
        struct list_head	wait_list;
    };

```

    DEFINED_SEMAPHORE(name)                                 定义一个 struct semaphore name 结构体 信号量， name 是信号量的名字， count 是信号量的值 设置为1 
    void semaphore_init(struct semaphore *sem, int val)     初始化信号量， val 是信号量的值
    void down(struct semaphore *sem)                        申请信号量，如果信号量的值大于0，则信号量的值减1，否则，进程挂起 ，直到信号量的值大于0； 老师讲的是 无法获得就会休眠，在休眠过程中无法被唤醒，即使 有信号发给这和进程也不处理，直到信号量的值大于0，才会被唤醒，继续执行
    void up(struct semaphore *sem)                          释放信号量，唤醒其他等待信号的进程 ； 释放信号量， 信号量的值加1，如果有进程在等待信号量，则唤醒进程，休眠过程中可能 会搜到信号而被唤醒，要判断 返回值，如果返回值是 -ERESTARTSYS，则表示进程被信号中断，需要重新执行，如果返回值是 0，则表示进程被唤醒，继续执行

    down_interruptible(struct semaphore *sem)  申请信号量，如果信号量的值大于0，则信号量的值减1，否则，进程挂起，直到信号量的值大于0，如果进程被中断，则返回 -ERESTARTSYS； 休眠过程中可能 会收到信号而被唤醒 （所有的信号都可以唤醒），要判断 返回值，
    down_killable(struct semaphore *sem)  申请信号量，如果信号量的值大于0，则信号量的值减1，否则，进程挂起，直到信号量的值大于0，如果进程被中断，则返回 -EINTR； 休眠过程中可能 会收到信号而被唤醒， 但 只能收到 fatal signal 唤醒，要判断 返回值，

    down_trylock(struct semaphore *sem)  申请信号量，如果信号量的值大于0，则信号量的值减1，否则，返回 0； 尝试 获得信号量，不会 休眠，返回值 0 获得信号量， 1 获得信号量失败

    down_interruptible_killable(struct semaphore *sem)  申请信号量，如果信号量的值大于0，则信号量的值减1，否则，进程挂起，直到信号量的值大于0，如果进程被中断，则返回 -ERESTARTSYS，如果进程被中断，则返回 -EINTR

    down_timeout(struct semaphore *sem, long timeout)  申请信号量，如果信号量的值大于0，则信号量的值减1，否则，进程挂起，直到信号量的值大于0，或者超时，如果超时，则返回 -ETIMEOUT； 休眠过程中可能 会收到信号而被唤醒，要判断 返回值，




互斥量 mutex
    sources/linux-rpi-6.6.y/include/linux/mutex.h
```C
struct mutex {
	atomic_long_t		owner;          // 互斥量持有者的进程ID, 
	raw_spinlock_t		wait_lock;      // 互斥量等待队列的锁
};

```


    互斥量是信号量的一种特例，互斥量只能取0和1两个值，0表示资源未被占用，1表示资源已被占用，互斥量只能被一个进程持有，其他进程只能等待持有者释放互斥量
    用在某个进程的 上下文里，用来保护 临界区，防止其他进程同时访问临界区

    DEFINED_MUTEX(name)                                 定义一个 struct mutex name 结构体 互斥量， name 是互斥量的名字
    void mutex_init(struct mutex *mutex)               初始化互斥量
    void mutex_lock(struct mutex *mutex)               申请互斥量，如果互斥量已经被占用，则进程挂起，直到互斥量被释放； 休眠过程中可能 会收到信号而被唤醒，要判断 返回值，
    void mutex_unlock(struct mutex *mutex)             释放互斥量，唤醒其他等待互斥量的进程； 释放互斥量， 互斥量的值加1，如果有进程在等待互斥量，则唤醒进程，休眠过程中可能 会搜到信号而被唤醒，要判断 返回值，
    int mutex_trylock(struct mutex *mutex)             尝试 申请互斥量，如果互斥量已经被占用，则返回 0，否则，返回 1； 尝试 获得互斥量，不会 休眠，返回值 0 获得互斥量， 1 获得互斥量失败

    int mutex_is_locked(struct mutex *mutex)           判断互斥量是否被占用，返回 1 表示被占用，返回 0 表示未被占用
    int mutex_is_locked_safe(struct mutex *mutex)      判断互斥量是否被占用，返回 1 表示被占用，返回 0 表示未被占用，如果互斥量被占用，则返回 -EDEADLK

    int mutex_lock_interruptible(struct mutex *mutex)  申请互斥量，如果互斥量已经被占用，则进程挂起，直到互斥量被释放，如果进程被中断，则返回 -ERESTARTSYS； 休眠过程中可能 会收到信号而被唤醒，要判断 返回值，
    int mutex_lock_killable(struct mutex *mutex)       申请互斥量，如果互斥量已经被占用，则进程挂起，直到互斥量被释放，如果进程被中断，则返回 -EINTR； 休眠过程中可能 会收到信号而被唤醒， 但 只能收到 fatal signal 唤醒，要判断 返回值，
    int mutex_lock_timeout(struct mutex *mutex, long timeout)  申请互斥量，如果互斥量已经被占用，则进程挂起，直到互斥量被释放，或者超时，如果超时，则返回 -ETIMEOUT； 休眠过程中可能 会收到信号而被唤醒，要判断 返回值，
    
    
    int mutex_lock_io(struct mutex *mutex)             申请互斥量，如果互斥量已经被占用，则进程挂起，直到互斥量被释放，如果进程被中断，则返回 -EINTR； 休眠过程中可能 会收到信号而被唤醒， 但 只能收到 fatal signal 唤醒，要判断 返回值，
    int mutex_lock_io_interruptible(struct mutex *mutex)  申请互斥量，如果互斥量已经被占用，则进程挂起，直到互斥量被释放，如果进程被中断，则返回 -ERESTARTSYS； 休眠过程中可能 会收到信号而被唤醒，要判断 返回值，


读写锁 rwlock
    读写锁是一种用于读多写少的场景下的锁，读写锁分为读锁和写锁，读锁可以被多个进程同时持有，写锁只能被一个进程持有，写锁不能被读锁持有，读锁可以被写锁持有


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

