#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <sys/mman.h>

#include <sys/ioctl.h>
#include <linux/spi/spidev.h>




void dc_pin_init(int number); //初始化DC引脚
int oled_init(void); //初始化oled
void OLED_DIsp_Test_dev_frameBuffer(void); //测试显示内容


// int oled_dc_pin_init(int number);
void oled_dc_pin_uninit(void);
void oled_set_dc_pin(int val);
void spi_write_datas( unsigned char *buf, int len); // 修改为 const
void oled_write_cmd_data(unsigned char uc_data, unsigned char uc_cmd);
void oled_write_datas_dev_frameBuffer( unsigned char *buf, int len);
void OLED_DIsp_Clear_dev_frameBuffer(void);
void OLED_DIsp_Set_Pos_dev_frameBuffer(int x, int y); // 添加声明
void OLED_DIsp_Char_dev_frameBuffer(int x, int y, unsigned char c);
void OLED_DIsp_String_dev_frameBuffer(int x, int y, char *str);


void app_spi_dev_oled_frameBuffer_main(int argc, char *argv[]);