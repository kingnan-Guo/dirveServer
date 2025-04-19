#include "app_input_device_system.h"
#define EVENT_DEVICE "/dev/input/event7"
// #define KEY_BACK 114
#define LOG_FILE "/tmp/key_events.log"

int app_input_device_system_init(int argc, char *argv[]) {
    int fd;
    struct input_id id;
    unsigned int evbit[EV_CNT/32 + 1];
    struct pollfd fds[1];
    nfds_t nfds = 1;
    FILE *log_file;

    const char *ev_names[] = {
        "EV_SYN ", "EV_KEY ", "EV_REL ", "EV_ABS ", "EV_MSC ", "EV_SW  ", "NULL   ",
        "NULL   ", "NULL   ", "NULL   ", "NULL   ", "NULL   ", "NULL   ", "NULL   ",
        "NULL   ", "NULL   ", "NULL   ", "EV_LED ", "EV_SND ", "NULL   ", "EV_REP ",
        "EV_FF  ", "EV_PWR "
    };

    // 检查参数
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <device> [noblock]\n", argv[0]);
        return -1;
    }

    // 打开设备
    int flags = O_RDWR;
    if (argc == 3 && !strcmp(argv[2], "noblock")) {
        flags |= O_NONBLOCK;
    }
    fd = open(argv[1], flags);
    if (fd == -1) {
        perror("Cannot open device");
        return -1;
    }

    // 打开日志文件
    log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        perror("Cannot open log file");
        close(fd);
        return -1;
    }

    // 获取设备信息
    if (ioctl(fd, EVIOCGID, &id) == 0) {
        printf("bustype: 0x%04x\n", id.bustype);
        printf("vendor : 0x%04x\n", id.vendor);
        printf("product: 0x%04x\n", id.product);
        printf("version: 0x%04x\n", id.version);
    } else {
        perror("Failed to get device ID");
    }

    // 获取支持的事件类型
    int len = ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), evbit);
    if (len > 0 && len <= sizeof(evbit)) {
        printf("Supported event types:\n");
        for (int i = 0; i < len; i++) {
            unsigned char byte = ((unsigned char *)evbit)[i];
            for (int j = 0; j < 8; j++) {
                if (byte & (1 << j) && ev_names[i * 8 + j]) {
                    printf("  %s\n", ev_names[i * 8 + j]);
                }
            }
        }
    } else {
        perror("Failed to get event bits");
    }

    // 配置 poll
    fds[0].fd = fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    // 主循环
    struct input_event ev;
    while (1) {
        int ret = poll(fds, nfds, 2000);
        if (ret > 0) {
            if (fds[0].revents & POLLIN) {
                while (1) {
                    ssize_t n = read(fd, &ev, sizeof(ev));
                    if (n == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break; // 非阻塞模式，无数据
                        }
                        perror("Read error");
                        break;
                    }
                    if (n != sizeof(ev)) {
                        fprintf(stderr, "Incomplete event read\n");
                        continue;
                    }

                    // 记录事件
                    time_t now = time(NULL);
                    char *time_str = ctime(&now);
                    time_str[strlen(time_str) - 1] = '\0'; // 移除换行

                    // 处理事件
                    if (ev.type == EV_KEY && ev.code == KEY_BACK) {
                        if (ev.value == 1) {
                            printf("[%s] KEY_BACK pressed\n", time_str);
                            fprintf(log_file, "[%s] KEY_BACK pressed\n", time_str);
                            // 示例：触发脚本
                            system("echo 'KEY_BACK pressed' >> /tmp/action.log");
                        } else if (ev.value == 0) {
                            printf("[%s] KEY_BACK released\n", time_str);
                            fprintf(log_file, "[%s] KEY_BACK released\n", time_str);
                            system("echo 'KEY_BACK released' >> /tmp/action.log");
                        }
                    } else if (ev.type == EV_SYN) {
                        printf("[%s] Sync event\n", time_str);
                        fprintf(log_file, "[%s] Sync event\n", time_str);
                    } else {
                        printf("[%s] type: %d, code: %d, value: %d\n", time_str, ev.type, ev.code, ev.value);
                        fprintf(log_file, "[%s] type: %d, code: %d, value: %d\n", time_str, ev.type, ev.code, ev.value);
                    }
                    fflush(log_file);
                }
            }
        } else if (ret == 0) {
            printf("Poll timeout\n");
        } else {
            perror("Poll error");
            if (errno == EINTR) continue;
            break;
        }
    }

    // 清理
    fclose(log_file);
    close(fd);
    return 0;
}

void app_input_device_system_main(int argc, char *argv[]) {
    app_input_device_system_init(argc, argv);
}
