#include "app_get_device_info.h"

void app_get_device_info_fasync_signal_handler(int signo){
    printf("app_get_device_info_fasync_signal_handler\n");


}


int app_get_device_info_fasync_init(int argc, char *argv[]){

    static int fd;
    struct input_id id; // 存储设备信息

    unsigned int evbit[EV_CNT/32 + 1]; // 0x1f 等于 31（十进制）

    


    char *ev_names[]  = {
		"EV_SYN ",
		"EV_KEY ",
		"EV_REL ",
		"EV_ABS ",
		"EV_MSC ",
		"EV_SW	",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"EV_LED ",
		"EV_SND ",
		"NULL ",
		"EV_REP ",
		"EV_FF	",
		"EV_PWR ",
    };
    

    if(argc < 2){
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    // 注册信号处理韩式 
    signal(SIGIO, app_get_device_info_fasync_signal_handler);

    if(argc == 3 && !strcmp(argv[2], "noblock")){
        fd = open(argv[1], O_RDWR | O_NONBLOCK);
    }
    else{
        fd = open(argv[1], O_RDWR);
    }
    
    if(fd == -1){
        printf("can not open file %s\n", argv[1]);
        return -1;
    }

    // 获取设备信息
    /**
     * ioctl (int __fd, unsigned long int __request, ...)
     *  __fd: 文件描述符
     * __request: ioctl命令; 对应驱动程序里的  cmd
     * ...: 传递给ioctl的参数; 对应驱动程序里的 arg
     * 
     * 为了获取 input_id 传入 cmd EVIOCGID 和 arg 0
     * 
     */
    int err = ioctl(fd, EVIOCGID, &id);
    if(err == 0){
        printf("bustype: %x\n", id.bustype);
        printf("vendor: %x\n", id.vendor);
        printf("product: %x\n", id.product);
        printf("version: %x\n", id.version);
    }

    /**
     * ./linux-rpi-6.6.y/drivers/input/evdev.c
     * 
     * 获取 输入设备的 evbit， evbit 表示这个设备 支持哪一个类型的 输入事件
     * 要使用 EVIOCGBIT(0, sizeof(evbit)) 命令; 如果要获得 keybit 则使用 EVIOCGBIT(1, sizeof(keybit))； dev->keybit
     * 
     * 在 handle_eviocgbit() 函数中，
     * 
     * 
     * evbit 每一位 对应里面 一种类型
     * evbit 存储的是一个位图，每一位 (bit) 对应一个 EV_* 事件类型
     * 
     * 
     */


    char byte;

    int len = ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);

    if(len > 0 && len <= sizeof(evbit)){
        printf("Supported event types:\n");
        for (size_t i = 0; i < len; i++)
        {
            byte = ((unsigned char *)evbit)[i];
            for (int j = 0; j < 8; j++){
                if (byte & (1 << j)) {
                    printf("  %s is supported\n", ev_names[i * 8 + j]);
                }
            }
        }

    }

    fcntl(fd, F_SETOWN, getpid()); // 设置 文件描述符的 owner 为当前进程
    unsigned int flags = fcntl(fd, F_GETFL);// 取出 文件描述符的 flag
    fcntl(fd, F_SETFL, flags | FASYNC); // 设置 文件描述符的 flag 为 FASYNC
    

    int count = 0;
    while (1)
    {
		printf("main loop count = %d\n", count++);
		sleep(2);
    }
    



    return 0;
}

void app_get_device_info_fasync_main(int argc, char *argv[]){
    app_get_device_info_fasync_init(argc, argv);
};


// ./app_get_device_info_fasync /dev/input/event0


// "EV_SYN",                   
// "EV_KEY",
// "EV_REL",
// "EV_ABS",
// "EV_MSC",
// "EV_SW",
// "NULL",
// "NULL",
// "NULL",
// "NULL",
// "NULL",
// "EV_LED",
// "EV_SND",
// "NULL",
// "EV_REP",
// "EV_FF",
// "EV_PWR",
// "EV_FF_STATUS",
// "EV_MAX",
// "EV_CNT"


// "EV_SYN"                    
// "EV_KEY"
// "EV_REL"
// "EV_ABS"
// "EV_MSC"
// "EV_SW"
// "NULL"
// "NULL"
// "NULL"
// "NULL"
// "NULL"
// "EV_LED"
// "EV_SND"
// "NULL"
// "EV_REP"
// "EV_FF"
// "EV_PWR"
// "EV_FF_STATUS"
// "EV_MAX"
// "EV_CNT"


// "EV_SYN"			0x00
// "EV_KEY"			0x01
// "EV_REL"			0x02
// "EV_ABS"			0x03
// "EV_MSC"			0x04
// "EV_SW"			0x05
// "NULL"
// "NULL"
// "NULL"
// "NULL"
// "NULL"
// "EV_LED"			0x11
// "EV_SND"			0x12
// "NULL"
// "EV_REP"			0x14
// "EV_FF"			0x15
// "EV_PWR"			0x16
// "EV_FF_STATUS"	0x17
// "EV_MAX"			0x1f
// "EV_CNT"			(EV_MAX+1)
