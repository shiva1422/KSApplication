cmake_minimum_required(VERSION 3.18.1)

#TODO right place and way
add_compile_definitions(VK_USE_PLATFORM_ANDROID_KHR)

add_library(ksvulkan STATIC KSVulkan.cpp vulkan_wrapper.cpp)

target_link_libraries(ksvulkan PRIVATE -DVK_USE_PLATFORM_ANDROID_KHR)
