#include "my_resources.h"


static struct my_resources my_board_n = {
    .pin = GROUP_PIN(3, 1)
};

struct my_resources * get_resources(void)
{
    return &my_board_n;
}