# cmake_minimum_required(VERSION 3.29)
# project(kernel_module)

# # 设置标准
# set(CMAKE_C_STANDARD 99)

# # 添加内核头文件路径
# include_directories(
#     /lib/modules/$(uname -r)/build/include
#     /lib/modules/$(uname -r)/build/include/uapi
#     /lib/modules/$(uname -r)/build/arch/x86/include
# )

# # 源文件
# set(SOURCES main.c)

# # 自定义目标构建
# add_custom_target(kernel_module ALL
#     COMMAND make -C /lib/modules/$(uname -r)/build M=${CMAKE_CURRENT_SOURCE_DIR} modules
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     VERBATIM
# )

# # 清理命令（更名为 clean_kernel_module）
# add_custom_target(clean_kernel_module
#     COMMAND make -C /lib/modules/$(uname -r)/build M=${CMAKE_CURRENT_SOURCE_DIR} clean
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     VERBATIM
# )


cmake_minimum_required(VERSION 3.29)
project(DirvesServer)

set(CMAKE_CXX_STANDARD 20)

# 指定包含目录
include_directories(

    # 模块 ===========
    modules/dirveServerLog   
    modules/DRIVETEMP
    modules/app_interrupt_sleep_wake_up_poll      

)


file(GLOB_RECURSE MODULE  "modules/*.*" )

file(GLOB_RECURSE SOURCES main.cpp)

add_executable(DirvesServer ${SOURCES} ${MODULE})



