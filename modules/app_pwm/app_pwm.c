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

    int start_duty = 0;
    int end_duty = 100;
    start_duty = atoi(argv[2]);
    snprintf(buffer, sizeof(buffer), "%d", start_duty);


    if(write(fd, buffer, strlen(buffer)) < 0){
        printf("write %s failed\n", argv[1]);
        return -1;
    }
    // close(fd);
    // 呼吸灯


    
    int duty = start_duty;
    int dir = 1;  // 1=上升, -1=下降

    while (1) {
        snprintf(buffer, sizeof(buffer), "%d", duty);
        if (write(fd, buffer, strlen(buffer)) < 0) {
            perror("write failed");
            break;
        }

        // 更新占空比
        duty += dir;
        if (duty >= 100) {
            duty = 100;
            dir = -1; // 开始下降
        } else if (duty <= 0) {
            duty = 0;
            dir = 1; // 开始上升
        }

        usleep(20000); // 每 20ms 更新一次亮度 ≈ 50Hz 呼吸灯节奏
    }

    close(fd);



    return 0;
}



void app_pwm_main(int argc, char *argv[]){
    app_pwm_init(argc, argv);
}