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


#include <linux/types.h>
#include <errno.h>




int app_virtual_spi_master_main(int argc, char *argv[]);