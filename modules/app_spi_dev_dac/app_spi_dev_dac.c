#include "app_spi_dev_dac.h"

#define SPI_IOC_WR 123

/**  main  /dev/spidevB.D  <val> */
int app_spi_dac_init(int argc, char *argv[]){

    int fd;
    int val;
    int status;// 返回值



    if(argc != 3){
        fprintf(stderr, "Usage: %s  /dev/spidevB.D   <val>\n", argv[0]);
        return -1;
    }

    // 写入16 位数据到时序图同时读出 16位数据； 要使用 ioctrl
    fd= open(argv[1], O_RDWR);
    if(fd < 0){
        perror("open error");
        return -1;
    }

    val = strtoul(argv[2], NULL, 0);// 将字符串转换为无符号长整数（unsigned long int）。


    status = ioctl(fd, SPI_IOC_WR, &val); // 发送 SPI 消息，使用 ioctl 函数将 xfer 数组传递给 SPI 驱动程序。
    if(status < 0){
        perror("ioctl error");
        close(fd);
        return -1;
    }


    printf("val  =  %d \n", val);


    close(fd);
    return 0;
}

void app_spi_dev_dac_main(int argc, char *argv[]){
    app_spi_dac_init(argc, argv);
}


/**
 要传输的16 位 数据里面
 最高 4 位 0 ，低 2位 0， 所以 只需构造中间 10 位 
 LSB                    MSB       
 0 0                    0 0 0 0
 

 所以在使用  main  /dev/spidevB.D  <val> 的时候 ，传入的 val 要左移 2 位， 保留中间的 10 位
    例如： 0x1234 传入的 val = 0x1234 << 2 = 0x12340
    
    保留中间的 10 位
    例如： 0x1234 &= 0xFFC； 保留中间的 10 位， 0x1234 = 0x1234 & 0xFFC = 0x1230

    0xFFC = 0b111111111100
 */