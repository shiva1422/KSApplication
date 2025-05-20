//
// Created by kalasoft on 12/23/21.
//

#include <Logger/KSLog.h>
#include <KSUI/View.h>
#include <Shader.h>
#include <unistd.h>
#include "GLRenderer.h"
#include "string"

#define TAGRENDER "GLUIRenderer"
#define GLSLTOSTRING(...) #__VA_ARGS__

#include "Shaders/vert.glsl"
#include "Graphics/KSUI/GL/GLView.h"


GLRenderer::~GLRenderer() {

    glContext.release();
}

bool GLRenderer::onRender() {
    //TODO backGroudn
   //KSLOGI("onRender","drawn");
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);

    if(content != nullptr)
    {
        View *view = static_cast<View *>(content);
        view->draw();
    }
    else
    {
       KSLOGE("Renderer","no contentView");
    }



    return glContext.swapBuffers();
}

bool GLRenderer::init()
{
    glContext.create();
    glContext.makeCurrent();
    return GLView::initializeUI();

}

bool GLRenderer::setSurfaceWindow(KSWindow &window)
{
    KSLOGD(TAGRENDER,"gl Surface (%d,%d)",window.getWidth(),window.getHeight());
    bool res  = glContext.setWindow(window);
    enableBlending();
   return res;
}

void GLRenderer::enableBlending() {

    glEnable(GL_BLEND);//TODO global management;
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void GLRenderer::disableBlending() {

    glDisable(GL_BLEND);
}

void GLRenderer::setContent(void *view) {
    Renderer::setContent(view);
    ((View *)content)->setRenderer(this);
}

bool GLRenderer::initOffScreenShared(GLRenderer *sharedRenderer, int width, int height) {

    return glContext.createSharedOffScreenContext(sharedRenderer->getGLContext(),width,height);

}


