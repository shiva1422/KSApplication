cmake_minimum_required(VERSION 3.18.1)

set(SHADERC_DIR ${ANDROID_NDK}/sources/third_party/shaderc)

#TODO right place and way
add_compile_definitions(VK_USE_PLATFORM_ANDROID_KHR)

include_directories(${SHADERC_DIR}/include)

add_library(ksvulkan STATIC KSVulkan.cpp KSVkDevice.cpp vulkan_wrapper.cpp ShaderManager.cpp Renderers/KSVkRenderer.cpp)

set(shaderc ${SHADERC_DIR}/obj/local/${CMAKE_ANDROID_ARCH_ABI}/libshaderc_combined.a)
target_link_libraries(ksvulkan  PRIVATE -DVK_USE_PLATFORM_ANDROID_KHR ${shaderc})
