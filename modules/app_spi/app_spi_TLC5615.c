#include "app_spi_TLC5615.h"



/**  main  /dev/spidevB.D  <val> */
int app_spi_TLC5615_init(int argc, char *argv[]){

    int fd;
    int val;
    struct spi_ioc_transfer xfer[1];// spi_ioc_transfer 结构体数组，表示 SPI 传输的配置和数据。
    int status;// 返回值

    unsigned char tx_buf[2];// 发送缓冲区
    unsigned char rx_buf[2];// 接收缓冲区

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

    val << 2; // 左移 2 位; bint0, bit1  = 0b00
    val &=0xFFC; //保留中间的 10 位; 0xFFC = 0b111111111100


    tx_buf[1] = val & 0xFF; // 取低 8 位
    tx_buf[0] = (val >> 8) & 0xFF; // 取高 8 位

    // 先传输 tx_buf[0] 然后传输 tx_buf[1]；
    // tx_buf[0] 存入高 8 位， tx_buf[1] 存入低 8 位


    
    memset(xfer, 0, sizeof(xfer));// 清零 xfer 数组，确保所有字段都被初始化为零。

    xfer[0].tx_buf = tx_buf;// 要发送的数据
    xfer[0].rx_buf = rx_buf;// 接收数据的缓冲区
    xfer[0].len = 2;// 传输的字节数, 2个 字节


    status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer); // 发送 SPI 消息，使用 ioctl 函数将 xfer 数组传递给 SPI 驱动程序。
    if(status < 0){
        perror("ioctl error");
        close(fd);
        return -1;
    }



    /**
    接收到的数据； 先接收 rx_buf[0] 然后接收 rx_buf[1]；
    数据是 一位一位传输过来的， 先读到 高8位，然后读到底八位

    LSB                    MSB       
    0 0                    0 0 0 0

    其中前 4 个是0，后 2 个是 0


    */

    printf("tx_buf: 0x%02x 0x%02x\n", tx_buf[0], tx_buf[1]);
    val = (rx_buf[0]<<8) | rx_buf[1];// 将高 8 位左移 8 位, 然后 与低 8 位进行或运算，得到完整的 16 位数据。

    printf("val  =  %d", val);


    close(fd);
    return 0;
}

void app_spi_TLC5615_main(int argc, char *argv[]){
    app_spi_TLC5615_init(argc, argv);
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