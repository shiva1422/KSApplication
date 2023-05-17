cmake_minimum_required(VERSION 3.10.2)

set(UI_SRCS
        View.cpp
        ViewGroup.cpp
        LinearLayout.cpp
        GL/GLView.cpp
        GL/GLImageView.cpp
        Renderer/Renderer.cpp
        Renderer/GLUIRenderer.cpp
        )

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. ./ksgraphics)

add_library(ksui STATIC ${UI_SRCS})

target_link_libraries(ksui ksgraphics)
