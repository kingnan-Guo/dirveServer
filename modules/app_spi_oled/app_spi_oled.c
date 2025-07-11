#include "app_spi_oled.h"


//为0 表示命令，为1表示数据
#define OLED_CMD 	0
#define OLED_DATA 	1


static int fd; // SPI 文件描述符
static int dc_pin_num; // DC 引脚号




void dc_pin_init(int number){
static int dc_pin_num; // DC 引脚号
    dc_pin_num = number;
    // 初始化 DC 引脚
    char cmd[100];
    // echo 509 > /sys/class/gpio/export 是 将 509 号引脚导出为 GPIO； 
    sprintf(cmd, "echo %d > /sys/class/gpio/export", number);
    system(cmd);// 导出 GPIO 引脚

    // 设置引脚方向
    sprintf(cmd, "echo out > /sys/class/gpio/gpio%d/direction", number);
    system(cmd);
}


// 初始化 oled
int oled_init(){
	oled_write_cmd_data(0xae,OLED_CMD);//关闭显示

	oled_write_cmd_data(0x00,OLED_CMD);//设置 lower column address
	oled_write_cmd_data(0x10,OLED_CMD);//设置 higher column address

	oled_write_cmd_data(0x40,OLED_CMD);//设置 display start line

	oled_write_cmd_data(0xB0,OLED_CMD);//设置page address

	oled_write_cmd_data(0x81,OLED_CMD);// contract control
	oled_write_cmd_data(0x66,OLED_CMD);//128

	oled_write_cmd_data(0xa1,OLED_CMD);//设置 segment remap

	oled_write_cmd_data(0xa6,OLED_CMD);//normal /reverse

	oled_write_cmd_data(0xa8,OLED_CMD);//multiple ratio
	oled_write_cmd_data(0x3f,OLED_CMD);//duty = 1/64

	oled_write_cmd_data(0xc8,OLED_CMD);//com scan direction

	oled_write_cmd_data(0xd3,OLED_CMD);//set displat offset
	oled_write_cmd_data(0x00,OLED_CMD);//

	oled_write_cmd_data(0xd5,OLED_CMD);//set osc division
	oled_write_cmd_data(0x80,OLED_CMD);//

	oled_write_cmd_data(0xd9,OLED_CMD);//ser pre-charge period
	oled_write_cmd_data(0x1f,OLED_CMD);//

	oled_write_cmd_data(0xda,OLED_CMD);//set com pins
	oled_write_cmd_data(0x12,OLED_CMD);//

	oled_write_cmd_data(0xdb,OLED_CMD);//set vcomh
	oled_write_cmd_data(0x30,OLED_CMD);//

	oled_write_cmd_data(0x8d,OLED_CMD);//set charge pump disable 
	oled_write_cmd_data(0x14,OLED_CMD);//

	oled_write_cmd_data(0xaf,OLED_CMD);//set dispkay on

	return 0;
}




void OLED_DIsp_Test(void){
    int i;




    
}



 /** main /dev/spidevB.D <DC_Pin_number> */
int app_spi_oled_init(int argc, char *argv[]){

    if(argc != 3){
        printf(stderr, "Usage: %s  /dev/spidevB.D   <DC_pin_number> \n", argv[0]);
        return;
    }

    fd = open(argv[1], O_RDWR);
    if(fd < 0){
        perror("open error");
        return -1;
    }

    // 获取 DC 的引脚号
    int dc_pin = strtoul(argv[2], NULL, 0);

    // 初始化 DC 引脚
    dc_pin_init(dc_pin);

    // 初始化 SPI oled
    oled_init();

    // 显示测试内容
    OLED_DIsp_Test();

    // SPI 初始化代码可以在这里添加
    close(fd);

    return 0;
}


void app_spi_oled_main(int argc, char *argv[]){

    app_spi_oled_init(argc, argv);
};