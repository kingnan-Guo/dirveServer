#include "app_pwm.h"




// ./mian   /dev/my_pwm_0     30
int app_pwm_init(int argc, char *argv[]){
    printf("app_pwm_init\n");

    int fd;
    int err;
    fd = open(argv[1], O_RDWR | O_NONBLOCK);
    if(fd < 0){
        printf("open %s failed\n", argv[1]);
        return -1;
    }

    printf("app_pwm_init ok\n");

    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%d", atoi(argv[2]));


    if(write(fd, buffer, strlen(buffer)) < 0){
        printf("write %s failed\n", argv[1]);
        return -1;
    }
    // close(fd);

    return 0;
}



void app_pwm_main(int argc, char *argv[]){
    app_pwm_init(argc, argv);
}