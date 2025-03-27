#include "app_interrupt_sleep_wake_up_fasync.h"


static int fd;

// sig 是 信号
void sig_func(int sig){
    int val;
    read(fd, &val, sizeof(val)); // 读取文件内容
    printf("read: %d\n", val);
    printf("sig: %d\n", sig);
}



int app_interrupt_sleep_wake_up_fasync_init(int argc, char *argv[]){

    int val;
    int ret;



    if(argc < 2){
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    // 打开文件
    fd = open(argv[1], O_RDWR);// O_RDWR 读写方式打开文件
    if(fd == -1){
        printf("Error: \n");
        return 1;
    }

    // 注册信号
    signal(SIGIO, sig_func);

    // 设置文件描述符为异步通知
    fcntl(fd, F_SETOWN, getpid());// F_SETOWN 告诉驱动程序，驱动程序有数据是了，发给 pid 进程
    int flags = fcntl(fd, F_GETFL);// F_GETFL 获取文件状态标志
    fcntl(fd, F_SETFL, flags | FASYNC);// F_SETFL 设置文件状态标志, FASYNC 异步通知


    while (1)
    {
        printf("app_interrupt_sleep_wake_up_fasync \n");
        sleep(2);
    }


    close(fd);
    return 0;
}

void app_interrupt_sleep_wake_up_fasync_main(int argc, char *argv[]){
    app_interrupt_sleep_wake_up_fasync_init(argc, argv);
};