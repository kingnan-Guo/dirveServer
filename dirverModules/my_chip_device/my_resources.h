
#pragma once


#define GROUP(X) (X >> 16)
#define PIN(X) (X&0xffff)
#define GROUP_PIN(group, pin) ((group << 16) | pin)
/**
 * 老师局的例子
 * 
 * bit[31:16] = group  为 分组
 * bit[15:0] = pin 为 该组的第几个引脚
 * 
 */


struct my_resources
{
    int pin;
};

struct my_resources * get_resources(void);
