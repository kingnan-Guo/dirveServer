cmake_minimum_required(VERSION 3.29)
project(dirveServer)

set(CMAKE_CXX_STANDARD 20)


include_directories(
    modules/dirveServerLog
    modules/DRIVETEMP
)
#add_executable(dirveServer main.cpp)

file(GLOB_RECURSE SOURCES main.cpp "modules/*.*" "lib/*.*")

add_executable(dirveServer ${SOURCES})


# # 指定头文件目录
# target_include_directories(dirveServer PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/module/log
#         ${CMAKE_CURRENT_SOURCE_DIR}/module
#         ${CMAKE_CURRENT_SOURCE_DIR}/lib
# )