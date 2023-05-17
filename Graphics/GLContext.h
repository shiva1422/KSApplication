//
// Created by kalasoft on 10/5/21.
//

#ifndef KALASOFT_GLCONTEXT_H
#define KALASOFT_GLCONTEXT_H

#include "EGL/egl.h"
#include "KSWindow.h"

#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

//TODO destroy,sharedContext

class GLContext {

public:
    bool create();

    bool setWindow(KSWindow &window);//surface

    bool makeCurrent();

    void printInfo();

    void clear();

    int getWidth(){return width;}

    int getHeight(){return height;}

    bool swapBuffers()
    {
        //?err
        eglSwapBuffers(eglDisplay,eglSurface);
        return true;

    }

    static GLenum getError(const char* tag);

private:
    int32_t width,height;
    EGLConfig config = nullptr ;
    EGLDisplay eglDisplay = EGL_NO_DISPLAY;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    EGLContext eglContext = EGL_NO_CONTEXT;

};


#endif //KALASOFT_GLCONTEXT_H
