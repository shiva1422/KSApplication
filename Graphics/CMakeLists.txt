cmake_minimum_required(VERSION 3.31.0)


set(GRAPHICS_SRCS
        Texture/GLTexture.cpp
        GLContext.cpp
        KSWindow.cpp
        Display.cpp
        Shader.cpp
        GLFrameBuffer.cpp
        )


add_library(ksgraphics STATIC ${GRAPHICS_SRCS})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../Core/Media/KSImage ./ksimage)


#add_subdirectory(Vulkan ./ksvulkan)

target_link_libraries(ksgraphics  ksimage GLESv3 EGL)
