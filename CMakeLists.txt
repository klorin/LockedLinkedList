cmake_minimum_required(VERSION 3.13)
project(untitled12)

set(CMAKE_CXX_STANDARD 11)

add_executable(untitled12 main.cpp ThreadSafeList.cpp ThreadSafeList.h Mutex.h Mutex.cpp)