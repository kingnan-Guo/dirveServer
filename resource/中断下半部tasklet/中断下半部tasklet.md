中断下半部tasklet
    分支
        dirver_raspberry_interrupt_sleep_wake_up_tasklet_v0.1.11

    文件
        ./modules/app_interrupt_sleep_wake_up_tasklet/app_interrupt_sleep_wake_up_tasklet.c
        
        interrupt_sleep_wake_up_tasklet.c




中断下半部
    1、中断不能嵌套
    2、越快越好
    3、在 中断上半部 执行的时候， 中断是被禁止的
    4、在 中断下半部 执行的时候， 中断是被允许的，中断是被 使能的


在处理硬件中断后

