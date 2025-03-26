#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "string.h"
#include <unistd.h>
#include <poll.h>

void app_interrupt_sleep_wake_up_poll_main(int argc, char *argv[]);