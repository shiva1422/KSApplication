//
// Created by kalasoft on 10/5/21.
//

#include <Logger/KSLog.h>
#include "GLContext.h"
#include "initializer_list"
#define LOGTAG "GLContext"
#include "KSWindow.h"

//TODO choose config right
bool GLContext::create()
{

    const EGLint attribs[]={EGL_RENDERABLE_TYPE,EGL_OPENGL_ES3_BIT,EGL_BLUE_SIZE, 8,EGL_GREEN_SIZE, 8,EGL_RED_SIZE, 8, EGL_SURFACE_TYPE, EGL_WINDOW_BIT,EGL_NONE};
    const EGLint context_attribs[]={EGL_CONTEXT_CLIENT_VERSION,3,EGL_NONE};
    EGLint numConfigs;
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(eglDisplay == EGL_NO_DISPLAY)
    {
        KSLog::error(LOGTAG, "create - no display");
        return false;
    }
    if(!eglInitialize(eglDisplay, nullptr, nullptr))//can pass variable to get the result opengl versions
    {
        KSLog::error(LOGTAG, "create - egl initialize failed");
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
        KSLog::error(LOGTAG, "eglChooseConfig failed ");
        return false;

    }
    if (config == nullptr)
    {
        KSLog::error(LOGTAG, "Unable to initialize EGLConfig");
        return false;
    }

    if((eglContext = eglCreateContext(eglDisplay, config, nullptr, context_attribs))==EGL_NO_CONTEXT)
    {
        KSLog::error(LOGTAG, "context creation failed");
        return false;
    }


    KSLog::info(LOGTAG, "created");

    return true;
}

bool GLContext::setWindow(KSWindow &window)
{
    if(window.get() == nullptr)
    {
        KSLog::error(LOGTAG,"cant set null window");
        return false;
    }

    const EGLint surfaceAttribs[]={EGL_WIDTH,window.getWidth(),EGL_HEIGHT,window.getHeight(),EGL_NONE};
   /* EGLint format;
    if(! eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format))
    {
        //gets the id in format of the display;
        KSLog::error(LOGTAG, "egl getConfig attrib failed");
        return false;
    }
    ANativeWindow_setBuffersGeometry(window,0,0,format);*///?
    eglSurface = eglCreateWindowSurface(eglDisplay, config,window.get(), nullptr);//surfaceAttribs/////
    if(eglSurface == EGL_NO_SURFACE)
    {
        KSLog::error(LOGTAG,"window surface creation failed");
        return false;
    }

    eglQuerySurface(eglDisplay,eglSurface, EGL_WIDTH, &width);
    eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &height);

    //glDisable(GL_DEPTH_TEST);
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    // glEnable(GL_CULL_FACE);
    // glShadeModel(GL_SMOOTH);
    KSLog::debug(LOGTAG,"surface dimensions(%d,%d)",width,height);

    makeCurrent();

    glViewport(0,0,width,height);
    printInfo();


    return true;

}

bool GLContext::makeCurrent()
{
    if (eglMakeCurrent(eglDisplay,eglSurface,eglSurface,eglContext ) == EGL_FALSE)
    {
        KSLog::error(LOGTAG,"Unable to eglMakeCurrent");
        return false;
    }
    return true;
}

void GLContext::printInfo()
{
    auto opengl_info ={GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
    for (auto name : opengl_info)
    {
        auto info = glGetString(name);
        KSLog::info(LOGTAG,"OpenGL Info: %s", info);
    }

    KSLog::info(LOGTAG,"surface Width %d and height %d",width,height);

}

//TODO log debug only
GLenum GLContext::getError(const char *tag) {
    //TODO bestway alsot separete gl and egl
    GLenum glError = glGetError();

    switch (glError)
    {
        case GL_INVALID_ENUM: {
            KSLog::error(tag, "OPENGL ERROR - INVALID ENUM");
        }
            break;
        case GL_INVALID_VALUE: {
            KSLog::error(tag, "OPENGL ERROR - INVALID VALUE");
        }
            break;
        case GL_INVALID_OPERATION:
        {
            KSLog::error(tag, "OPENGL ERROR - INVALID OPERATION");
        }
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:

        {
            KSLog::error(tag,"OPENGL ERROR - INVALIDE FRAMEBUFFER OPERATION - CURRENTLY BOUND FRAMEBUFFER IS NOT FRAME BUFFER COMPLETE");
        }
        case GL_OUT_OF_MEMORY:
        {
            KSLog::error(tag,"OPENGL ERROR-NOT ENOUGH MEMORY");
        }
        case GL_NO_ERROR:
        {
            KSLog::error(tag,"NO OPENGL ERROR");
        }
            break;
        default:
            KSLog::error(tag, "NO OPENGL EROOR OR the opengl error not listed in printglerror");

    }



    //also print any egl errors
    EGLenum eglError=eglGetError();
    switch(eglError)
    {
        case EGL_BAD_DISPLAY :
        {
            KSLog::error(tag,"EGL ERROR - BAD DISPLAY-DISPLAY IS NOT AN EGL DISPLAY CONNECTION");

        }
            break;
        case EGL_BAD_SURFACE :
        {
            KSLog::error(tag,"EGL ERROR - BAD SURFACE -SURFACE NOT AN EGL  DRAWING SURFACE");

        }
            break;
        case EGL_CONTEXT_LOST :
        {
            KSLog::error(tag,"EGL ERROR - EGL CONTEXT LOST -REINITILIZING EVERYTHING AGAIN NEEDED");

        }
            break;
        case EGL_NOT_INITIALIZED:
        {
            KSLog::error(tag,"EGL ERROR - EGL NOT INITIALIZED - DISPLAY HAS NOT BEEN INITIALIZED");

        }
            break;
        default:
            KSLog::error(tag,"NO EGL ERROR OR ERROR IS NOT LISTED IN pringGlError function");

    }
    return glError;
}
