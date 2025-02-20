cmake_minimum_required(VERSION 3.31.0)

set(UI_SRCS
        View.cpp
        ViewGroup.cpp
        LinearLayout.cpp
        GL/GLView.cpp
        GL/GLImageView.cpp
        GL/RectView.cpp
        GL/GLSliderView.cpp
        GL/HSIImageView.cpp
        Renderer/Renderer.cpp
        Renderer/GLRenderer.cpp
        Renderer/GLUIOffScreenRenderer.cpp
        ClickListener.cpp
        )

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. ./ksgraphics)

add_subdirectory(Text ./kstextlib)

add_library(ksui  STATIC ${UI_SRCS})

target_link_libraries(ksui ksgraphics kstextlib)
