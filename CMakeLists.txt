cmake_minimum_required(VERSION 3.0)
project(os_labs C)

set(CMAKE_C_STANDARD 11)

find_library(PTHREAD_LIBRARY pthread)

add_executable(monitor monitor.c)
target_link_libraries(monitor ${PTHREAD_LIBRARY})

add_executable(signal signal.c)
