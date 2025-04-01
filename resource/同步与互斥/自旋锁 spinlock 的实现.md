2025/04/01 17:10
# 自旋锁 spinlock 的实现
    分支
        dirver_raspberry_XXXXX_vX.X.X

    文件
        ./modules/app_XXX/app_XXX.c
        ./dirverModules/dirverModules/XXX.c

# 定义

自旋锁 spinlock
    1、自旋锁是一种忙等待锁，当线程获取锁失败时，它会一直循环等待，直到获取到锁为止。


    app -> read -> dirver -> read -> spin_lock -> xxxx -> spin_unlock 

    1、当app 调 dirver 的 read 的时候，如果 read 中 有 调用 spin_lock, 到了 XXX
    2、这时如果有中断过来 打断 XXX，这时后 中断 可以执行，打断了 app 的进程
    3、如果 不想让 中断打断 进程 那么 就需要 spin_lock_irqsave, 关中断  保存中断状态，同时 禁止抢占 preempt_disable

在SMP 系统中
    1、要让多个 cpu 中 只能有一个获得 临街资源，使用原子变量就可以实现，但是还要保证公平，先到先得
    2、当 cpu0 获得自旋锁，这时 有 cpu1 来排队， cpu2 也来排队，要 cpu1 先 获得锁，cpu2 后获得锁；
    3、这个时候 需要有个取号机，给 cpu1 取号 1，给 cpu2 取号 2，然后 cpu1 先获得锁，cpu2 等待，cpu1 释放锁，cpu2 获得锁， 有一个叫号机 唤醒 



sources/linux-rpi-6.6.y/include/linux/spinlock_types.h

```C

typedef struct spinlock {
	union {
		struct raw_spinlock rlock;

#ifdef CONFIG_DEBUG_LOCK_ALLOC
# define LOCK_PADSIZE (offsetof(struct raw_spinlock, dep_map))
		struct {
			u8 __padding[LOCK_PADSIZE];
			struct lockdep_map dep_map;
		};
#endif
	};
} spinlock_t;





static inline void arch_spin_lock(arch_spinlock_t *lock)
{
	unsigned long tmp;
	u32 newval;
	arch_spinlock_t lockval;

	prefetchw(&lock->slock);
	__asm__ __volatile__(
"1:	ldrex	%0, [%3]\n"         // 使用 %3 取出  (&lock->slock) 的值，存入 %0； 也就是  raw_spinlock -> arch_spinlock_t -> slock
"	add	%1, %0, %4\n"           // %1 = %0 + (1 << TICKET_SHIFT) // %1 = lockval.tickets.next = lockval.tickets.next + (1 << TICKET_SHIFT); (1 << TICKET_SHIFT) 是 1 左移 16 位，也就是 65536
"	strex	%2, %1, [%3]\n"        // 使用 %3 取出  (&lock->slock) 的值，存入 %2； 也就是  raw_spinlock -> arch_spinlock_t -> ；但是 不一定成功
"	teq	%2, #0\n"                // 判断 %2 是否等于 0，如果等于 0，表示 strex 指令执行成功，将 %1 的值写入 %3 所指向的内存地址，否则表示 strex 指令执行失败，继续执行 1 标签处的代码； 如果  1 、2、3 步骤被别人抢先了，那么写回操作失败，那么就会继续执行 1 标签处的代码，直到成功为止，
"	bne	1b"
	: "=&r" (lockval), "=&r" (newval), "=&r" (tmp)
	: "r" (&lock->slock), "I" (1 << TICKET_SHIFT)
	: "cc");

	while (lockval.tickets.next != lockval.tickets.owner) {// 如果 lockval.tickets.next 不等于 lockval.tickets.owner，表示当前 cpu 还没有获得锁，需要继续等待
		wfe();// 原地休息，cpu 低功耗运行
		lockval.tickets.owner = READ_ONCE(lock->tickets.owner);// 重新读取锁的拥有者，防止其他 cpu 修改了锁的拥有者
	}
    // 如果 lockval.tickets.next 等于 lockval.tickets.owner，表示当前 cpu 已经获得锁，可以继续执行后面的代码
	smp_mb();
}



// 解锁 spinlock
static inline void arch_spin_unlock(arch_spinlock_t *lock)
{
	smp_mb();
	lock->tickets.owner++;// 在解锁的时候  lock->tickets.owner++，表示当前 cpu 已经释放锁，其他 cpu 可以尝试获得锁
	dsb_sev();
}

```

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

