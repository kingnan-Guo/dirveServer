#include "app_interrupt_sleep_wake_up_poll.h"


int app_interrupt_sleep_wake_up_poll_init(int argc, char *argv[]){
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
    

    char buffer[1024];
    int val;
    while (1)
    {
        // length = read(fd, buffer, sizeof(buffer));
        // printf("Read %d bytes from %s\n", length, argv[1]);
        // printf("%s\n", buffer);

        read(fd, &val, sizeof(val));
        printf("Read %d bytes from %s\n", length, argv[1]);
        printf("get button : 0x%x\n", val);
    }
    


    close(fd);
}

void app_interrupt_sleep_wake_up_poll_main(int argc, char *argv[]){
    app_interrupt_sleep_wake_up_poll_init(argc, argv);
};