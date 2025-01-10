#include <iostream>
#include "dirveServerLog.h"
#include "DRIVETEMP.h"


int main(int arch, char* argv[]) {
    //初始化日志
    dirveServerLog * log = new dirveServerLog();
    log->initLog(); //初始化日志
    // std::cout << "Hello, World!" << std::endl;


    DRIVETEMP_MAIN(arch, argv);
    return 0;
}
