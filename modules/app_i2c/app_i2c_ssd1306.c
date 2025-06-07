#include "app_i2c.h"


static int fd;
int app_i2c_ssd1306_init(int argc, char *argv[]) {
int ret = 0;
    char buffer[100] = {0};

    if (argc < 2) {
        printf("Usage: %s <i2c bus> [data]\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        printf("open %s failed\n", argv[1]);
        return -1;
    }

    if (argc == 3) {
    }

    close(fd);
    return 0;
}


void app_i2c_ssd1306_main(int argc, char *argv[]){
    app_i2c_ssd1306_init(argc, argv);
}