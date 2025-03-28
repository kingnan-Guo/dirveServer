#include <stdio.h>
#include <stdlib.h>

// #include "app_interrupt_sleep_wake_up_circle.h"
// #include "app_interrupt_sleep_wake_up_poll.h"
// #include "app_interrupt_sleep_wake_up_fasync.h"
#include "app_interrupt_sleep_wake_up_noblock.h"

int main(int argc, char *argv[]){

    // app_interrupt_sleep_wake_up_circle_main(argc, argv);
    
    // app_interrupt_sleep_wake_up_poll_main(argc, argv);

    // app_interrupt_sleep_wake_up_fasync_main(argc, argv);

    app_interrupt_sleep_wake_up_noblock_main(argc, argv);

    return 0;

}