cmake_minimum_required(VERSION 3.31.0)


find_library(log-lib log)

add_library(events  Events/CustomEvents.cpp)

#set(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

set(APP_SRCS
        KSApplication.cpp
        JNI/AppJavaCalls.cpp
        ../../Core/KSIO/AssetManager.cpp
        ../../Core/KSIO/FileManager.cpp
        IO/KSAssetReader.cpp
        Events/AndroidEvents.cpp
        Events/CustomEvents.cpp
        AppUtils.cpp
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

add_library(ksapp STATIC ${APP_SRCS})

#add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../KSLogger ./kslogger)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../Graphics/KSUI ./ksui)

target_include_directories(ksapp  PUBLIC  ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(ksapp  ksui jnigraphics android log )

