#include <stdio.h>
#include <stdlib.h>

// #include "app_interrupt_sleep_wake_up_circle.h" // 环形缓冲区
// #include "app_interrupt_sleep_wake_up_poll.h" // poll
// #include "app_interrupt_sleep_wake_up_fasync.h"// fasync
// #include "app_interrupt_sleep_wake_up_noblock.h"// 阻塞和非阻塞
// #include "app_interrupt_sleep_wake_up_timer.h"// 定时器
// #include "app_interrupt_sleep_wake_up_tasklet.h"// tasklet
// #include "app_interrupt_sleep_wake_up_workqueue.h"// 工作队列
// #include "app_interrupt_sleep_wake_up_threadedirq.h" // 内核线程中断
// #include "app_mmap.h"
// #include "app_assembly.h"
// #include "app_get_device_info.h"
// #include "app_input_device_system.h"

#include "app_gpio_system.h"



int main(int argc, char *argv[]){

    // app_interrupt_sleep_wake_up_circle_main(argc, argv);
    
    // app_interrupt_sleep_wake_up_poll_main(argc, argv);

    // app_interrupt_sleep_wake_up_fasync_main(argc, argv);

    // app_interrupt_sleep_wake_up_timer_main(argc, argv);

    // app_interrupt_sleep_wake_up_tasklet_main(argc, argv);

    // app_interrupt_sleep_wake_up_workqueue_main(argc, argv);

    // app_interrupt_sleep_wake_up_threadedirq_main(argc, argv);

    // app_mmap_main(argc, argv);


    // app_assembly_main(argc, argv);

    // app_get_device_info_main(argc, argv);
    // app_get_device_info_poll_main(argc, argv);
    // app_get_device_info_select_main(argc, argv);
    // app_input_device_system_main(argc, argv);
    // app_input_device_system_poll_main(argc, argv);

    app_gpio_system_main(argc, argv);
    

    return 0;

}