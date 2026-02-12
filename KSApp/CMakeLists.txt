cmake_minimum_required(VERSION 3.31.0)


find_library(log-lib log)

find_package(game-activity REQUIRED CONFIG)
#find_package(game-text-input REQUIRED CONFIG)


#set(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

set(APP_SRCS
        KSApplication.cpp
        JNI/AppJavaCalls.cpp
        JNI/JNIImage.cpp
        JNI/KSNativeActivityBridge.cpp

        ../../Core/KSIO/AssetManager.cpp
        ../../Core/KSIO/FileManager.cpp
        IO/KSAssetReader.cpp
        Events/AndroidEvents.cpp
        Events/AppEvent.cpp
        Events/AppEvents.cpp
        Events/MotionEvent.cpp
        AppUtils.cpp
       # ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

add_library(ksapp STATIC ${APP_SRCS})

#add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../KSLogger ./kslogger)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../Graphics/KSUI ./ksui)

target_include_directories(ksapp  PUBLIC     ${game-activity_INCLUDE_DIRS} )


target_link_libraries(ksapp  ksui jnigraphics android log game-activity::game-activity_static )


