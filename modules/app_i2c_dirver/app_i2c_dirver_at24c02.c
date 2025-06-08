#include "app_i2c.h"



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

    printf("argv[1] = %s\n", argv[1]);

    // fd = open(argv[1], O_RDWR | O_NONBLOCK);
    fd = open_i2c_dev(argv[1][0], filename, sizeof(filename), 0);
    
    if (fd == -1) {
        printf("open %s failed\n", argv[1]);
        return -1;
    }



    // 设置从设备地址
    if(set_slave_addr(fd, dev_addr, 1)){
            printf("set_slave_addr failed\n");
            return -1;
    }

    // 读写数据
    if(argv[2][0] == 'w'){
        str = argv[3]; // 获取要写入的字符串


        req.tv_sec = 0; // 秒
        req.tv_nsec = 20000000; // 纳秒


        while(*str){
            // 在 mem_addr 写 *str
            // 写完一个 字节后，mem_addr 自增 1， str 自增 1
            
            if(i2c_smbus_write_byte_data(fd, mem_addr, *str)){// 写数据
                printf("i2c_smbus_write_byte_data failed\n");
                return -1;
            }

            // 写完后 延时 10ms
            nanosleep(&req, NULL);

            mem_addr++;
            str++;
        }

        ret = i2c_smbus_write_byte_data(fd, mem_addr, 0);// 写数据   写入字符串结束符 0

        if (ret)
        {
            printf("i2c_smbus_write_byte_data failed\n");

            return -1;
        }
        
    }
    else if(argv[2][0] == 'r'){
        ret = i2c_smbus_read_i2c_block_data(fd, mem_addr, sizeof(buffer), buffer);// 读取数据
        if (ret < 0) {
            
            return -1;
        }

        printf("read: %s\n", buffer);
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