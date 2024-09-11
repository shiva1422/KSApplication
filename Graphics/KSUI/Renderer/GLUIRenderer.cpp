//
// Created by kalasoft on 12/23/21.
//

#include <Logger/KSLog.h>
#include <KSUI/View.h>
#include <Shader.h>
#include <unistd.h>
#include "GLUIRenderer.h"
#include "string"

#define TAGRENDER "GLUIRenderer"
#define GLSLTOSTRING(...) #__VA_ARGS__

#include "Shaders/vert.glsl"
#include "Graphics/KSUI/GL/GLView.h"

bool GLUIRenderer::onRender() {
    //TODO backGroudn
   //KSLOGI("onRender","drawn");
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);

    //glUseProgram(texProg);
    if(content != nullptr)
    {
        View *view = static_cast<View *>(content);
        view->draw();
    }
    else
    {
       //KSLOGE("Renderer","no contentView");
    }



    return glContext.swapBuffers();
}

bool GLUIRenderer::init()
{

    std::string vert = "#version 310 es" ;
    vert += vertShader;
    KSLOGI("shader","%s",vertShader);
   // assert(false);
    glContext.create();
    glContext.makeCurrent();
    texProg = Shader::createProgram(vertShader,fragShader);
   // sleep(10);
    return GLView::initializeUI();

}

bool GLUIRenderer::setSurfaceWindow(KSWindow &window)
{
    KSLOGD(TAGRENDER,"gl Surface (%d,%d)",window.getWidth(),window.getHeight());
    bool res  = glContext.setWindow(window);
   return res;
}
