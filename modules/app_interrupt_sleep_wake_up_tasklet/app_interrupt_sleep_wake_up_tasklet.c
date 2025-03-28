#include "app_interrupt_sleep_wake_up_tasklet.h"


static int fd;


int app_interrupt_sleep_wake_up_tasklet_init(int argc, char *argv[]){

    int val;
    int ret;


    if(argc < 2){
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    // 打开文件
    fd = open(argv[1], O_RDWR | O_NONBLOCK);// O_RDWR 读写方式打开文件, O_NONBLOCK 非阻塞方式打开文件
    if(fd == -1){
        printf("can not open file %s\n", argv[1]);
        return -1;
    }


    // 设置文件描述符为异步通知
    // fcntl(fd, F_SETOWN, getpid());// F_SETOWN 告诉驱动程序，驱动程序有数据是了，发给 pid 进程
    // int flags = fcntl(fd, F_GETFL);// F_GETFL 获取文件状态标志
    // fcntl(fd, F_SETFL, flags | FASYNC);// F_SETFL 设置文件状态标志, FASYNC 异步通知


    // 在非阻塞 读数据
    for (size_t i = 0; i < 10; i++)
    {
        if(read(fd, &val, sizeof(val)) == 4){
            printf("O_NONBLOCK get value: %d\n", val);
        } else {
            printf("O_NONBLOCK get value: -1\n");
        }
    }

    // 设置 阻塞 读数据
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);// flags & ~O_NONBLOCK 设置为阻塞方式
    

    while (1)
    {
        printf("app_interrupt_sleep_wake_up_tasklet \n");

        // if(read(fd, &val, sizeof(val)) == 4){
        //     printf("BLOCK get value: %d\n", val);
        // } else {
        //     printf("BLOCK get value: -1\n");
        // }

        read(fd, &val, sizeof(val));
        printf("app_interrupt_sleep_wake_up_tasklet get value: %d\n", val); 

        // sleep(2);
    }


    close(fd);
    return 0;
}

void app_interrupt_sleep_wake_up_tasklet_main(int argc, char *argv[]){
    app_interrupt_sleep_wake_up_tasklet_init(argc, argv);
};