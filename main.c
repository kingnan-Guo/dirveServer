#include <stdio.h>
#include <stdlib.h>

// #include "app_interrupt_sleep_wake_up_circle.h"
// #include "app_interrupt_sleep_wake_up_poll.h"
// #include "app_interrupt_sleep_wake_up_fasync.h"
// #include "app_interrupt_sleep_wake_up_noblock.h"
#include "app_interrupt_sleep_wake_up_timer.h"

int main(int argc, char *argv[]){

    // app_interrupt_sleep_wake_up_circle_main(argc, argv);
    
    // app_interrupt_sleep_wake_up_poll_main(argc, argv);

    // app_interrupt_sleep_wake_up_fasync_main(argc, argv);

    app_interrupt_sleep_wake_up_timer_main(argc, argv);

    return 0;

}