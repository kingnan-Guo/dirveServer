#pragma once

struct my_operations
{
    int (*init) (int which);  // 初始化 LED ， which 哪个 LED，可能是 次设备号
    int (*ctl) (int which, char status);// 控制 LED ， which 哪个 LED，status 1 打开 0 关闭
};


// 单板  文件中 要实现这个函数
struct my_operations *my_board_operations(void);



