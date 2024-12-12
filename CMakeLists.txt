cmake_minimum_required(VERSION 3.29)
project(dirveServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(dirveServer main.cpp)

# 链接 Winsock 库
target_link_libraries(dirveServer ws2_32)
