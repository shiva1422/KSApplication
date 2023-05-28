cmake_minimum_required(VERSION 3.18.0)
project(Thread)
include_directories(../../)
add_executable(a.out main.cpp ../../Test/main.cpp ../LooperThread.cpp ../LooperThread.h)