2025/03/29 00:10
中断下半部tasklet
    分支
        dirver_raspberry_interrupt_sleep_wake_up_tasklet_v0.1.11

    文件
        ./modules/app_interrupt_sleep_wake_up_tasklet/app_interrupt_sleep_wake_up_tasklet.c
        
        interrupt_sleep_wake_up_tasklet.c



中断上半部
    1、request_irq(XXX_irq, xx_function); xx_function 中毒时调用的 函数
    2、可以使能 或 调度 下半部
    3、调度下半部就是 把一个 预先写好  tasklet 结构体 放入到 链表中



中断下半部
    1、中断不能嵌套 
    2、越快越好
    3、在 中断上半部 执行的时候， 中断是被禁止的
    4、在 中断下半部 执行的时候， 中断是被允许的，中断是被 使能的





流程
    1、先执行 硬件中断
        GPIO net tick 是 硬件中断
        在处理硬件中断后， 返回被中断的函数 之前，会去调用 软中断;
    
    2、软件中断 ;  eg: timer 中断
        a: 在内核里有个数组 softirq_vec[]，这个数组里存放着各种软中断的处理函数； 
        b:里面有很多 元素， 其中一个 元素  是  timer，这个元素里存放着 timer 的处理函数；也会有 tasklet 元素
        c: 比如说 某一项 tasklet 元素， 里面有 .action 函数，当 内核调用某一个硬件中断函数之后，会去调用 softirq_vec[] 数组里的每一个元素，当调用到 tasklet 元素 的时候，就会去调用 tasklet 元素里的 .action 函数；
        d: 例如 .action = tasklet_action 
        e: tasklet_action 会 从 一个链表里面 取出里面的每一项， 每一项是个 tasklet 结构体， tasklet 结构体 里面会有一个函数 .function



    softirq_vec[] 的大概结构是
        softirq_vec[] = {
            timer: {
                .action = timer_action
            },
            tasklet:{
                .action = tasklet_action
            },
            xxx: {
                .action = xxx_action
            },
        };



    tasklet_action 会 从 一个链表里面 取出里面的每一项， 每一项是个 tasklet 结构体， tasklet 结构体 里面会有一个函数 .function
        tasklet_action = {
            .function = tasklet_function
        };


    tasklet 链表 的结构是 
        tasklet_list = {struct tasklet { .function }} -> {struct tasklet { .function }}

        ;



    tasklet_function 是 tasklet 的处理函数，这个函数 是用户自定义的，用户可以在里面写自己的代码；

    tasklet_action 里面会去调用 tasklet_function，tasklet_function 是用户自定义的，用户可以在里面写自己的代码；




执行顺序
    0、 在执行函数的时候，有中断
    1、先执行 硬件中断
    2、软件中断 ; 如果这是有新的 硬件中断
    3、软件中断被打断
    4、执行 硬件中断
    5、硬件中断 执行完成， 还原 被打断的 软件 中断
    6、软件中断执行 完成
    7、还原 被打断的 函数， 继续 执行






# 定义 tasklet 结构体
    struct tasklet_struct {
        struct tasklet_struct *next; // 指向下一个 tasklet 结构体
        unsigned long state; // tasklet 的状态
        atomic_t count; // tasklet 的计数， 如果 count 不为 0，则 tasklet 不会被调度
        void (*func)(unsigned long); // tasklet 的处理函数
        unsigned long data; // tasklet 的数据
    }


    1、state 中有 2 位
        bit0: TASKLET_STATE_SCHED
            等于 1时 表示 已经执行了 tasklet_schedule 函数， tasklet 已经被调度了， 已经放入队列了， tasklet_schedule 会判断该位，如果已经等于 1 那么他就不会再次把 tasklet 放入队列了
        bit1: TASKLET_STATE_RUN
            等于 1 时 ，表示 正在运行 tasklet 的 func 函数，函数执行完 后内核你把该位 清 0
            内核在执行之前会把 bit1  设置 位 TASKLET_STATE_RUN， 执行完成之后 再把 bit1 清 0


    2、 count 是用来防止 tasklet 被重复调度的，如果 count 不为 0，则 tasklet 不会被调度；count 表示该 tasklet 是否使能
        等于 0 表示 使能，非 0 表示禁止，对于 count 非 0 的 tasklet，里面的 func 函数不会被珍惜下

    3、 func 是 tasklet 的处理函数，用户可以在里面写自己的代码



# 调度 tasklet
    1、static inline void tasklet_schedule(&tasklet); 把 tasklet 放入链表，并且 内核 设置它的 TASKLET_STATE_SCHED 状态 位 1
        tasklet_schedule 会判断 tasklet 的 state 是否等于 TASKLET_STATE_SCHED ，如果等于 1，则不会再次把 tasklet 放入队列中
        tasklet_schedule 会判断 tasklet 的 count 是否等于 0，如果等于 0，则不会把 tasklet 放入队列中


# kill tasklet
    1、extern void tasklet_kill(struct tasklet_struct *tasklet); 
       a :如果一个 tasklet 未被调度， tasklet_kill 会把他的 TASKLET_STATE_SCHED 状态 清零
       b: 如果一个 tasklet 已经被调度， tasklet_kill 会 等待他执行完毕， 把他的 TASKLET_STATE_SCHED 状态 清零； 等待的过程 可能是 会调用 tasklet_unlock_wait 函数，tasklet_unlock_wait 函数会等待 tasklet 的 TASKLET_STATE_RUN 状态 清零，然后才会返回
       c: 通常 在卸载驱动的时候 调用  tasklet_kill 函数，防止 tasklet 一直运行，导致系统崩溃

       d: 如果一个 tasklet 的 count 不等于 0，则 tasklet_kill 不会把他的 TASKLET_STATE_SCHED 状态 清零，也不会等待他执行完毕，也不会返回







# 内部机制











# Makefile
# # tasklet ---------------------
interrupt_sleep_wake_up_tasklet-y := $(MODULES_DIR)/interrupt_sleep_wake_up_tasklet/interrupt_sleep_wake_up_tasklet.o
obj-m := interrupt_sleep_wake_up_tasklet.o


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

