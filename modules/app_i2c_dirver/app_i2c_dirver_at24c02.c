#include "app_i2c.h"

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <smbus.h>
#include "i2cbusses.h"


static int fd;
char filename[20] = {0};


int app_i2c_dirver_at24c02_init(int argc, char *argv[]) {
    int ret = 0;
    char buffer[100] = {0};

    // 地址
    unsigned char dev_addr = 0x50; // AT24C02 的 I2C 地址
    // 存储空间的地址
    unsigned char mem_addr = 0x00; // 存储空间的起始地址

    unsigned char *str;// 存储字符串的指针
    struct timespec req;// 延时结构体

    if (argc < 2) {
        printf("Usage: %s <i2c bus> [data]\n", argv[0]);
        return -1;
    }

    // fd = open(argv[1], O_RDWR | O_NONBLOCK);
    fd = open_i2c_dev(argv[1], filename, sizeof(filename), 0);
    
    if (fd == -1) {
        printf("open %s failed\n", argv[1]);
        return -1;
    }



    // 设置从设备地址
   if(set_slave_addr(fd, dev_addr, 1)){
       printf("set_slave_addr failed\n");
        // close(fd);
        return -1;
   }

   // 读写数据


   if(argv[2][0] == 'w'){

   }

   else if(argv[2][0] == 'r'){
       
   }



    // if (argc == 3) {
    //     ret = write(fd, argv[2], strlen(argv[2]) + 1); // 写入字符串包括 \0
    //     if (ret < 0) {
    //         printf("write failed: %d\n", ret);
    //     } else {
    //         printf("wrote %d bytes\n", ret);
    //     }
    // } else {
    //     ret = read(fd, buffer, sizeof(buffer) - 1);
    //     if (ret < 0) {
    //         printf("read failed: %d\n", ret);
    //     } else {
    //         buffer[ret] = '\0'; // 确保字符串终止
    //         printf("read: %s\n", buffer);
    //     }
    // }

    return 0;
}


void app_i2c_dirver_at24c02_main(int argc, char *argv[]){
    app_i2c_dirver_at24c02_init(argc, argv);
}