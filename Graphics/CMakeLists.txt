cmake_minimum_required(VERSION 3.18.1)


set(GRAPHICS_SRCS
        Texture/GLTexture.cpp
        GLContext.cpp
        KSWindow.cpp
        Display.cpp
        Shader.cpp)


add_library(ksgraphics STATIC ${GRAPHICS_SRCS})

target_link_libraries(ksgraphics GLESv3 EGL)
