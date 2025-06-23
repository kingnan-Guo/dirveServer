/**
 * 发送接收数据 
 * 
 *  将 rx 和 tx 接到一起 然后 发送数据的时候就可以接收数据
 */
#include "app_uart.h"





int open_port(char *dev){
    int fd;

    fd = open(dev, O_RDWR | O_NOCTTY );// O_RDWR | O_NOCTTY 
    if (fd < 0) {
        perror("open_port: Unable to open serial port");
        return -1;
    }


    // 设置串口为阻塞状态
    int err = fcntl(fd, F_SETFL, 0);

    if(err < 0){
        perror("fcntl failed\n");
        close(fd);
        return -1;
    }
    return fd;
}


/**
 * 设置串口属性
 *  @param fd: 文件描述符
 *  @param nSpeed: 波特率
 *  @param nBits: 数据位
 *  @param nEvent: 校验位
 *  @param nStop: 停止位
 *  @return: 0 on success, -1 on failure
 * 
 * 设置波特率、数据位、校验位和停止位等串口属性
 * 
 * 
 */
int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop){
    struct termios newtio, oldtio;// 串口属性结构体

    // 获取当前串口属性, 并保存到 oldtio 中
    if(tcgetattr(fd, &oldtio) != 0) {
        perror("fcntl SetupSerial");
        return -1;
    }

    // 将串口属性结构体清零
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD; // 设置本地连接和接收使能
    newtio.c_cflag &= ~CSIZE; // 清除数据位设置
    newtio.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG); // 设置输入模式为原始模式
    newtio.c_oflag &= ~OPOST; // 设置输出模式为原始模式

    // 设置数据位
    switch ( nBits ) 
    {
        case 7:
            newtio.c_cflag |= CS7; // 设置为7位数据位
            break;
        case 8:
            newtio.c_cflag |= CS8; // 设置为8位数据位
            break;
        default:
            break;
    }







    switch( nEvent )
	{
	case 'O':// 奇校验
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
	break;
	case 'E': // 偶校验
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
	break;
	case 'N': 
		newtio.c_cflag &= ~PARENB;// 无奇偶校验
	break;
	}

	switch( nSpeed )
	{
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
	break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
	break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
	break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
	break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
	break;
	}
	
	if( nStop == 1 )
		newtio.c_cflag &= ~CSTOPB;
	else if ( nStop == 2 )
		newtio.c_cflag |= CSTOPB;
	
	newtio.c_cc[VMIN]  = 1;  /* 读数据时的最小字节数: 没读到这些数据我就不返回! */
	newtio.c_cc[VTIME] = 0; /* 等待第1个数据的时间: 
	                         * 比如VMIN设为10表示至少读到10个数据才返回,
	                         * 但是没有数据总不能一直等吧? 可以设置VTIME(单位是10秒)
	                         * 假设VTIME=1，表示: 
	                         *    10秒内一个数据都没有的话就返回
	                         *    如果10秒内至少读到了1个字节，那就继续等待，完全读到VMIN个数据再返回
	                         */

	tcflush(fd,TCIFLUSH);
	
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
		return -1;
	}



    return 0;
}



/*
 * ./main <dev>
 */
int app_uart_init(int argc, char *argv[]){

    int fd;
    int iRet;
    char buf[1024] = {0};


    /** 1 open 打开设备节点 */

    if(argc < 2){
        printf("argc error\n");
        return -1;
    }

    fd = open_port(argv[1]);
    if(fd < 0){
        printf("open %s err!\n", argv[1]);
        return -1;
    }


    /** 2 setup 设置设备节点属性
     * 115200
     * 8N1
     * RAW mode
     * renturn data immediately
     * 
     * 
     * 
     * 设置波特率   115200
     * 设置数据位   8
     *  设置校验位  N
     * 设置停止位   1
     * 设置数据流控制   N
     * 设置读写模式  RAW
     * 设置读写缓冲区   1024
     * 设置读写超时时间 0
     * 
    */

    iRet = set_opt(fd, 115200, 8, 'N', 1);
    if(iRet){
        printf("set port err!\n");
        return -1;
    }

    printf("Enter a char: ");

    /** 9 发送数据 */

    /** 10 接收数据 */

    /** 11 关闭设备节点 */

    while (1)
    {
        scanf("%c", &buf);
        iRet = write(fd, &buf, 1);// 发送数据
        iRet = read(fd, &buf, 1); // 接收数据
        if(iRet == 1){
            printf("get: %02x %c\n", buf[0], buf[0]);
        }
        else{
            printf("can not get data\n");
        }
    }
    
    close(fd);
    return 0;


    
}

void app_aurt_main(int argc, char *argv[]){
    app_uart_init(argc, argv);
}