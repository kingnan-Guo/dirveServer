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



void app_gpio_system_main(int argc, char *argv[]);