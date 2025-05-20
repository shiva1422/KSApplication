//
// Created by kalasoft on 12/23/21.
//

#ifndef KALASOFT_GLRENDERER_H
#define KALASOFT_GLRENDERER_H


#include <Graphics/GLContext.h>
#include "Renderer.h"

class GLRenderer : public Renderer {
    //TODO Make GLUIRenderer separete and this to GLUIRenderer//store thread id of this context;

public:


    ~GLRenderer()override;

    bool setSurfaceWindow(KSWindow &window);

    virtual void setWindow(KSWindow *window) override
    {
        Renderer::setWindow(window);
        KSWindow ksWindow;
        ksWindow.setWindow(window->get());//TODO optimize
        setSurfaceWindow(ksWindow);//TODO wtf?
    }

    int getWidth()override{return glContext.getWidth();}

    int getHeight()override{return glContext.getHeight();}

    void enableBlending();

    void disableBlending();


    bool initOffScreenShared(GLRenderer *sharedRenderer, int width, int Height);

    virtual bool init() override;

    GLContext* getGLContext(){return  &glContext;}

public:

    GLuint texProg = 0;

    void setContent(void *view) override;



protected:

    GLContext  glContext;

protected:

    virtual bool onRender() override;

    friend class KSApplication;

};


#endif //KALASOFT_GLRENDERER_H
