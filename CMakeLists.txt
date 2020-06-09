cmake_minimum_required(VERSION 3.14)
project(untitled12)

set(CMAKE_CXX_STANDARD 11)

add_executable(untitled12 listTest.cpp ThreadSafeList.cpp ThreadSafeList.h Mutex.h Mutex.cpp)