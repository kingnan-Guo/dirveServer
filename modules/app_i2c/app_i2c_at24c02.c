#include "app_i2c.h"


static int fd;
int app_i2c_at24c02(int argc, char *argv[]) {
    int ret = 0;

    char buffer[100];

    if(argc < 2) {
        printf("Usage: %s <i2c bus>\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR | O_NONBLOCK);
    if(fd == -1){
        printf("open %s failed\n", argv[1]);
        return -1;
    }


    if(argc == 3){
        ret = write(fd, argv[2], strlen(argv[2] + 1));
    }
    else {
        ret = read(fd, buffer, 100);
        printf("read %s\n", buffer);
    }

    close(fd);





    return 0;
}


void app_i2c_at24c02_main(int argc, char *argv[]){
    app_i2c_at24c02(argc, argv);
}