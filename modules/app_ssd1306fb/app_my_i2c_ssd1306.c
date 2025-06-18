// #include <stdio.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <string.h>


#include "app_ssd1306fb.h"



int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "用法: %s <文本>\n", argv[0]);
        return 1;
    }

    int fd = open("/dev/my_ssd1306_0", O_WRONLY);
    if (fd < 0) {
        perror("无法打开设备");
        return 1;
    }

    if (write(fd, argv[1], strlen(argv[1])) < 0) {
        perror("写入失败");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}


void app_my_i2c_ssd1306_main(int argc, char *argv[]){
    app_my_i2c_ssd1306_init(argc, argv);
}