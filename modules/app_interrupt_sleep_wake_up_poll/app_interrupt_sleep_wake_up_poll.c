#include "app_interrupt_sleep_wake_up_poll.h"


int app_interrupt_sleep_wake_up_poll_init(int argc, char *argv[]){

    int fd;
    int val;
    struct pollfd fds[1]; // 定义一个pollfd结构体数组. 一个文件描述符对应一个pollfd结构体
    int timeout_ms = 5000;
    int ret;


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


    fds[0].fd = fd;
    fds[0].events = POLLIN;

    while (1)
    {
        // poll 函数 用于等待文件描述符的变化; 参数 1: fds, 参数 2: nfds 是 fds 数组的长度, 参数 3: timeout
        ret = poll(fds, sizeof(fds)/sizeof(fds[0]), timeout_ms);

        if((ret == 1) && (fds[0].revents && POLLIN)){
            read(fd, &val, 4); // 读取文件内容
            printf("read: %d\n", val);
        }
        else {
            printf("timeout\n");
        }
    }
    


    close(fd);
    return 0;
}

void app_interrupt_sleep_wake_up_poll_main(int argc, char *argv[]){
    app_interrupt_sleep_wake_up_poll_init(argc, argv);
};
