#include <iostream>
#include "dirveServerLog.h"


int main() {
    //初始化日志
    dirveServerLog * log = new dirveServerLog();
    log->initLog(); //初始化日志
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
