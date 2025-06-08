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
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <time.h>
#include <sys/ioctl.h>


#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include "i2cbusses.h"


// 使用 内部的 i2c_driver 来读写取 eeprom at24c02
void app_i2c_dirver_at24c02_main(int argc, char *argv[]);

