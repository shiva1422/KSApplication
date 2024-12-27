cmake_minimum_required(VERSION 3.31.0)

find_library(log-lib log)
add_library(logger STATIC KSLog.cpp)
target_link_libraries(logger android  ${log-lib})