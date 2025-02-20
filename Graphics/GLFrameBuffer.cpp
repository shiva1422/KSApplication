//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 1/4/25.
//

#include <Logger/KSLog.h>
#include "GLFrameBuffer.h"
#include "KSUI/Renderer/Renderer.h"


#define TAGLOG "GLFrameBuffer"

GLFrameBuffer::GLFrameBuffer()
{
    glGenFramebuffers(1,&id);
}


GLFrameBuffer::~GLFrameBuffer() {

    if(glIsTexture(texId))
    {
        glDeleteTextures(1,&texId);
    }
    if(glIsRenderbuffer(depthBufId))
    {
        glDeleteRenderbuffers(1,&depthBufId);
    }

    glDeleteFramebuffers(1,&id);
}

GLFrameBuffer::GLFrameBuffer(int width, int height):GLFrameBuffer()
{
    setDims(width,height);
}

void GLFrameBuffer::setDims(int width, int height)
{
    this->width = width;
    this->height = height;
}
void GLFrameBuffer::configureColorBuffer()
{

    if(glIsTexture(texId))
    {
        glDeleteTextures(1,&texId);
    }
    glGenTextures(1,&texId);
    glBindTexture(GL_TEXTURE_2D,texId);
    //  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,0);//just make GL_RGB for color only.
    glTexStorage2D(GL_TEXTURE_2D,1,GL_RGBA8,width,height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
    GLContext::getError(TAGLOG);
}
void GLFrameBuffer::configureDepthBuffer()
{
    //optional DepthBuffer if depth testing needed
    if(glIsRenderbuffer(depthBufId))
    {
        glDeleteRenderbuffers(1,&depthBufId);
    }
    glGenRenderbuffers(1,&depthBufId);
    glBindRenderbuffer(GL_RENDERBUFFER,depthBufId);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_COMPONENT,GL_RENDERBUFFER,depthBufId);
}

void GLFrameBuffer::configure()
{


    configureColorBuffer();
    glBindFramebuffer(GL_FRAMEBUFFER,id);
    // configureDepthBuffer();////optional remove if not needed
    //configure the frame buffer.
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texId,0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // draw to the specified  buffers attached to this frame buffer listed in above line.

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        KSLOGE(TAGLOG,"error configure FrameBuffer");
    }else
    {
        KSLOGD(TAGLOG,"configure complete");
    }
    setToDefault();//

}
void GLFrameBuffer::makeActive()
{
    glBindFramebuffer(GL_FRAMEBUFFER,id);
    glViewport(0,0,width,height);///change immediate after fbo rendering done to default screen
    //  Loge("FrameBuffer::makeActive","width %d and height %d",width,height);
}

void GLFrameBuffer::setToDefault(int width, int height) {

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,width < 0 ? Renderer::getDisplayMetrics().screenWidth : width,height < 0 ? Renderer::getDisplayMetrics().screenHeight : height);
    //TODO Instead Renderer::getWidth,height(can be obtained as this will match the Window size)::which is equicalient to this threads GLCOntext::Suface:Width


}


