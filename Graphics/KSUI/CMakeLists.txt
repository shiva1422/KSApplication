cmake_minimum_required(VERSION 3.31.0)

set(UI_SRCS
        View.cpp
        ViewGroup.cpp
        LinearLayout.cpp
        GL/GLView.cpp
        GL/GLImageView.cpp
        GL/RectView.cpp
        Renderer/Renderer.cpp
        Renderer/GLUIRenderer.cpp
        ClickListener.cpp
        )

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. ./ksgraphics)

add_library(ksui STATIC ${UI_SRCS})

target_link_libraries(ksui ksgraphics)
