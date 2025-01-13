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

cmake_minimum_required(VERSION 3.5)
project(hello_world_driver)

# 内核源码目录（根据实际内核版本设置）
set(KDIR /lib/modules/3.10.0-1160.119.1.el7.x86_64/build)



# 驱动源码文件
set(DRIVER_SRC main.c)

# 编译目标
add_custom_target(
    build ALL
    COMMAND make -C ${KDIR} M=${CMAKE_CURRENT_SOURCE_DIR} modules
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building Linux Kernel Module"
)

# 清理目标
# add_custom_target(
#     clean
#     COMMAND make -C ${KDIR} M=${CMAKE_CURRENT_SOURCE_DIR} clean
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     COMMENT "Cleaning Linux Kernel Module"
# )