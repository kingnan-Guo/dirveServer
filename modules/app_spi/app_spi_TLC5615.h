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




void app_spi_TLC5615_main(int argc, char *argv[]);