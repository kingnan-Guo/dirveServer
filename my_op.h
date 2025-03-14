#pragma once

struct my_operations
{
    // int num;
    int (*init) (int which);  // 初始化 LED ， which 哪个 LED，可能是 次设备号
    int (*ctl) (int which, int status);// 控制 LED ， which 哪个 LED，status 1 打开 0 关闭
    int (*read) (int which, char *status); // 读取 LED ， which 哪个 LED
    int (*exit) (int which); // 退出 LED ， which 哪个 LEDs
};


// 单板  文件中 要实现这个函数
struct my_operations *get_board_operations(void);



