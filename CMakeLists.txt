cmake_minimum_required(VERSION 3.6)
project(producer-consumer-mutex-demo)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
find_package(Threads)

add_executable(main ${SOURCE_FILES})
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})
