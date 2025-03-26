#include "app_interrupt_sleep_wake_up_circle.h"


int init(int argc, char *argv[]){

    int fd;
    int length;
    char status;
    if(argc < 2){
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    // 打开文件
    fd = open(argv[1], O_RDWR);
    if(fd == -1){
        printf("Error: \n");
        return 1;
    }


    close(fd);
    return 0;
}

void app_interrupt_sleep_wake_up_poll_main(int argc, char *argv[]){
    init(argc, argv);
};