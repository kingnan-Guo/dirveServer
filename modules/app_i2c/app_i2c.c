#include "app_i2c.h"


#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define FRAMEBUFFER_SIZE (OLED_WIDTH * (OLED_HEIGHT / 8))

int app_i2c_init(int argc, char *argv[]) {
int fd;
    unsigned char *buffer = calloc(FRAMEBUFFER_SIZE, 1);
    if (!buffer) {
        perror("Failed to allocate buffer");
        return 1;
    }

    // 创建测试图案：每隔一行填充0xFF
    for (int i = 0; i < FRAMEBUFFER_SIZE; i += OLED_WIDTH) {
        memset(buffer + i, 0xFF, OLED_WIDTH);
        i += OLED_WIDTH; // 跳过一行
    }

    fd = open("/dev/ssd1306", O_WRONLY);
    if (fd < 0) {
        perror("Failed to open /dev/ssd1306");
        free(buffer);
        return 1;
    }

    if (write(fd, buffer, FRAMEBUFFER_SIZE) != FRAMEBUFFER_SIZE) {
        perror("Failed to write to /dev/ssd1306");
        close(fd);
        free(buffer);
        return 1;
    }

    printf("Test pattern written to OLED\n");
    close(fd);
    free(buffer);
    return 0;

}

void app_i2c_main(int argc, char *argv[]){
    app_i2c_init(argc, argv);
}