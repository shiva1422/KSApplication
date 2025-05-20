//
// Created by kalasoft on 10/5/21.
//

#include <Logger/KSLog.h>
#include <assert.h>
#include "GLContext.h"
#include "initializer_list"
#include "KSWindow.h"

#define LOGTAG "GLContext"

//TODO choose config right

GLContext::~GLContext() {

    if(eglSurface != EGL_NO_SURFACE)
    {
        eglDestroySurface(eglDisplay,eglSurface);
    }
    eglDestroyContext(eglDisplay,eglContext);
}

bool GLContext::create()
{

    const EGLint attribs[]={EGL_RENDERABLE_TYPE,EGL_OPENGL_ES3_BIT,EGL_BLUE_SIZE, 8,EGL_GREEN_SIZE, 8,EGL_RED_SIZE, 8, EGL_SURFACE_TYPE, EGL_WINDOW_BIT,EGL_NONE};
    const EGLint context_attribs[]={EGL_CONTEXT_CLIENT_VERSION,3,EGL_NONE};
    EGLint numConfigs;
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(eglDisplay == EGL_NO_DISPLAY)
    {
       KSLOGE(LOGTAG, "create - no display");
        return false;
    }
    if(!eglInitialize(eglDisplay, nullptr, nullptr))//can pass variable to get the result opengl versions
    {
       KSLOGE(LOGTAG, "create - egl initialize failed");
        return false;
    }
    //available configs
    /*   std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
         assert(supportedConfigs);
         eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);
    assert(numConfigs);
     auto i = 0;
    GraphicsLog("numConfigs %d",numConfigs);
      for (; i < numConfigs; i++) {
           auto& cfg = supportedConfigs[i];
           EGLint r, g, b, d,a;
           if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r)   &&
               eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
               eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b)  &&
               eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
               r == 8 && g == 8 && b == 8) {//&&d=0 removed
               config = supportedConfigs[i];
               break;
           }
       }
      if (i == numConfigs) {
       GraphicsLog("no configuration match");
       config = supportedConfigs[0];
     }*/


    if(!eglChooseConfig(eglDisplay, attribs, &config,1, &numConfigs)||numConfigs<=0)
    {    //////TODO chose the first config
       KSLOGE(LOGTAG, "eglChooseConfig failed ");
        return false;

    }
    if (config == nullptr)
    {
       KSLOGE(LOGTAG, "Unable to initialize EGLConfig");
        return false;
    }

    if((eglContext = eglCreateContext(eglDisplay, config, nullptr, context_attribs))==EGL_NO_CONTEXT)
    {
       KSLOGE(LOGTAG, "context creation failed");
        return false;
    }


    KSLOGI(LOGTAG, "created");

    return true;
}

bool GLContext::setWindow(KSWindow &window)
{
    if(window.get() == nullptr)
    {
       KSLOGE(LOGTAG,"cant set null window");
        return false;
    }

    const EGLint surfaceAttribs[]={EGL_WIDTH,window.getWidth(),EGL_HEIGHT,window.getHeight(),EGL_NONE};
   /* EGLint format;
    if(! eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format))
    {
        //gets the id in format of the display;
       KSLOGE(LOGTAG, "egl getConfig attrib failed");
        return false;
    }
    ANativeWindow_setBuffersGeometry(window,0,0,format);*///?
    eglSurface = eglCreateWindowSurface(eglDisplay, config,window.get(), nullptr);//surfaceAttribs/////
    if(eglSurface == EGL_NO_SURFACE)
    {
       KSLOGE(LOGTAG,"window surface creation failed");
        return false;
    }

    eglQuerySurface(eglDisplay,eglSurface, EGL_WIDTH, &width);
    eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &height);

    //glDisable(GL_DEPTH_TEST);
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    // glEnable(GL_CULL_FACE);
    // glShadeModel(GL_SMOOTH);
    KSLOGD(LOGTAG,"surface dimensions(%d,%d)",width,height);

    makeCurrent();


    printInfo();

  //  ANativeWindow_release(window.get());
    return true;

}

bool GLContext::makeCurrent()
{
    bool res = eglMakeCurrent(eglDisplay,eglSurface,eglSurface,eglContext );
    glViewport(0,0,width,height);
    return res;
}

bool GLContext::createSharedOffScreenContext(const GLContext *sharedContext, int bufferWidth, int bufferHeight) {

    const EGLint attribs[]={EGL_RENDERABLE_TYPE,EGL_OPENGL_ES3_BIT,EGL_BLUE_SIZE, 8,EGL_GREEN_SIZE, 8,EGL_RED_SIZE, 8, EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,EGL_NONE};
    const EGLint context_attribs[]={EGL_CONTEXT_CLIENT_VERSION,3,EGL_NONE};
    EGLint numConfigs;
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(eglDisplay == EGL_NO_DISPLAY)
    {
        KSLOGE(LOGTAG, "create - no display");
        return false;
    }
    if(!eglInitialize(eglDisplay, nullptr, nullptr))//can pass variable to get the result opengl versions
    {
        KSLOGE(LOGTAG, "create - egl initialize failed");
        return false;
    }
    if(!eglChooseConfig(eglDisplay, attribs, &config,1, &numConfigs)||numConfigs<=0)
    {    //////TODO chose the first config
        KSLOGE(LOGTAG, "eglChooseConfig failed ");
        return false;

    }
    if (config == nullptr)
    {
        KSLOGE(LOGTAG, "Unable to initialize EGLConfig");
        return false;
    }

    if((eglContext = eglCreateContext(eglDisplay, config, sharedContext->eglContext, context_attribs))==EGL_NO_CONTEXT)
    {
        KSLOGE(LOGTAG, "context creation failed");
        return false;
    }

    const EGLint surfaceAttribs[]={EGL_WIDTH,bufferWidth,EGL_HEIGHT,bufferHeight,EGL_NONE};
    eglSurface = eglCreatePbufferSurface(eglDisplay,config, surfaceAttribs);
    if(eglSurface == EGL_NO_SURFACE)
    {
        KSLOGE(LOGTAG,"Error Creating PBuffer Surface");
        return false;
    }

    eglQuerySurface(eglDisplay,eglSurface, EGL_WIDTH, &width);
    eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &height);

    printInfo();

    assert(width == bufferWidth && height == bufferHeight);

    return true;

}

void GLContext::printInfo()
{
    auto opengl_info ={GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
    for (auto name : opengl_info)
    {
        auto info = glGetString(name);
        KSLOGI(LOGTAG,"OpenGL Info: %s", info);
    }

    KSLOGI(LOGTAG,"surface Width %d and height %d",width,height);

}

//TODO log debug only
GLenum GLContext::getError(const char *tag) {
    //TODO bestway alsot separete gl and egl
    GLenum glError = glGetError();

    switch (glError)
    {
        case GL_INVALID_ENUM: {
           KSLOGE(tag, "OPENGL ERROR - INVALID ENUM");
        }
            break;
        case GL_INVALID_VALUE: {
           KSLOGE(tag, "OPENGL ERROR - INVALID VALUE");
        }
            break;
        case GL_INVALID_OPERATION:
        {
           KSLOGE(tag, "OPENGL ERROR - INVALID OPERATION");
        }
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:

        {
           KSLOGE(tag,"OPENGL ERROR - INVALIDE FRAMEBUFFER OPERATION - CURRENTLY BOUND FRAMEBUFFER IS NOT FRAME BUFFER COMPLETE");
        }
        case GL_OUT_OF_MEMORY:
        {
           KSLOGE(tag,"OPENGL ERROR-NOT ENOUGH MEMORY");
        }
        case GL_NO_ERROR:
        {
           KSLOGE(tag,"NO OPENGL ERROR");
        }
            break;
        default:
           KSLOGE(tag, "NO OPENGL EROOR OR the opengl error not listed in printglerror");

    }



    //also print any egl errors//TODO separate EGLError;
    EGLenum eglError=eglGetError();
    switch(eglError)
    {
        case EGL_BAD_DISPLAY :
        {
           KSLOGE(tag,"EGL ERROR - BAD DISPLAY-DISPLAY IS NOT AN EGL DISPLAY CONNECTION");

        }
            break;
        case EGL_BAD_SURFACE :
        {
           KSLOGE(tag,"EGL ERROR - BAD SURFACE -SURFACE NOT AN EGL  DRAWING SURFACE");

        }
            break;
        case EGL_CONTEXT_LOST :
        {
           KSLOGE(tag,"EGL ERROR - EGL CONTEXT LOST -REINITILIZING EVERYTHING AGAIN NEEDED");

        }
            break;
        case EGL_NOT_INITIALIZED:
        {
           KSLOGE(tag,"EGL ERROR - EGL NOT INITIALIZED - DISPLAY HAS NOT BEEN INITIALIZED");

        }
            break;
        default:
           KSLOGE(tag,"NO EGL ERROR OR ERROR IS NOT LISTED IN pringGlError function");

    }
    return glError;
}

bool GLContext::createSharedContext(GLContext *sharedContext) {

    const EGLint attribs[]={EGL_RENDERABLE_TYPE,EGL_OPENGL_ES3_BIT,EGL_BLUE_SIZE, 8,EGL_GREEN_SIZE, 8,EGL_RED_SIZE, 8, EGL_SURFACE_TYPE, EGL_WINDOW_BIT,EGL_NONE};
    const EGLint context_attribs[]={EGL_CONTEXT_CLIENT_VERSION,3,EGL_NONE};
    EGLint numConfigs;
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(eglDisplay == EGL_NO_DISPLAY)
    {
        KSLOGE(LOGTAG, "create - no display");
        return false;
    }
    if(!eglInitialize(eglDisplay, nullptr, nullptr))//can pass variable to get the result opengl versions
    {
        KSLOGE(LOGTAG, "create - egl initialize failed");
        return false;
    }
    if(!eglChooseConfig(eglDisplay, attribs, &config,1, &numConfigs)||numConfigs<=0)
    {    //////TODO chose the first config
        KSLOGE(LOGTAG, "eglChooseConfig failed ");
        return false;

    }
    if (config == nullptr)
    {
        KSLOGE(LOGTAG, "Unable to initialize EGLConfig");
        return false;
    }

    if((eglContext = eglCreateContext(eglDisplay, config, sharedContext->eglContext, context_attribs))==EGL_NO_CONTEXT)
    {
        KSLOGE(LOGTAG, "context creation failed");
        return false;
    }

   // printInfo();

    return true;
}

void GLContext::release() {

    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, nullptr);
}



