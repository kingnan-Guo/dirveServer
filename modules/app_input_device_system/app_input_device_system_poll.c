#include "app_input_device_system.h"

#define INPUT_IOC_MAGIC 'M'
#define IOCTL_RESET_COUNT _IOW(INPUT_IOC_MAGIC, 0x01, unsigned int)
#define IOCTL_GET_EVENT_PATH _IOR(INPUT_IOC_MAGIC, 0x05, char[256])

#define MAX_PATH 256
#define BUFFER_SIZE 256
int app_input_device_system_poll_init(int argc, char *argv[])
{
    int fd_char, fd_input;
    char event_path[MAX_PATH];
    struct pollfd pfd;
    char buffer[BUFFER_SIZE];
    struct input_event ev;
    ssize_t n;

    if (argc != 2) {
        fprintf(stderr, "用法: %s <字符设备路径>\n", argv[0]);
        fprintf(stderr, "示例: %s /dev/my_input_device\n", argv[0]);
        return 1;
    }

    fd_char = open(argv[1], O_RDWR);
    if (fd_char < 0) {
        perror("无法打开字符设备");
        return 1;
    }

    if (ioctl(fd_char, IOCTL_GET_EVENT_PATH, event_path) < 0) {
        perror("无法获取事件路径");
        close(fd_char);
        return 1;
    }
    printf("输入设备路径: %s\n", event_path);

    strcpy(event_path, "/dev/input/event7");
    fd_input = open(event_path, O_RDONLY | O_NONBLOCK);
    if (fd_input < 0) {
        perror("无法打开输入设备");
        close(fd_char);
        return 1;
    }

    unsigned int button_idx = 0;
    if (ioctl(fd_char, IOCTL_RESET_COUNT, &button_idx) < 0) {
        perror("无法重置按键计数");
    } else {
        printf("已重置按键 %u 的计数\n", button_idx);
    }

    pfd.fd = fd_char;
    pfd.events = POLLIN;

    printf("等待按键事件（按 Ctrl+C 退出）...\n");

    while (1) {
        int ret = poll(&pfd, 1, 1000);
        if (ret < 0) {
            perror("轮询失败");
            break;
        }
        if (ret > 0 && (pfd.revents & POLLIN)) {
            n = read(fd_char, buffer, BUFFER_SIZE - 1);
            if (n < 0) {
                perror("读取失败");
                continue;
            }
            buffer[n] = '\0';
            printf("字符设备事件: %s", buffer);
        }

        n = read(fd_input, &ev, sizeof(ev));
        if (n == sizeof(ev)) {
            if (ev.type == EV_KEY) {
                printf("输入事件: type=%u, code=%u, value=%d\n",
                       ev.type, ev.code, ev.value);
            }
        } else if (n < 0 && errno != EAGAIN) {
            perror("无法读取输入事件");
        }
    }

    close(fd_input);
    close(fd_char);
}

// 主函数，用于调用输入设备系统轮询的初始化函数
void app_input_device_system_poll_main(int argc, char *argv[]){
    app_input_device_system_poll_init(argc, argv);
}