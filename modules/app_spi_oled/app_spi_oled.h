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
void OLED_DIsp_Test(void); //测试显示内容


void app_spi_oled_main(int argc, char *argv[]);