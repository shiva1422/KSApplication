//
// Created by kalasoft on 12/23/21.
//

#ifndef KALASOFT_GLUIRENDERER_H
#define KALASOFT_GLUIRENDERER_H


#include <Graphics/GLContext.h>
#include "Renderer.h"

class GLUIRenderer : public Renderer {
    //TODO Make GLUIRenderer separete and this to GLUIRenderer//store thread id of this context;


public:

    bool setSurfaceWindow(KSWindow &window);

    virtual void setWindow(KSWindow *window) override
    {
        Renderer::setWindow(window);
        KSWindow ksWindow;
        ksWindow.setWindow(window->get());//TODO optimize
        setSurfaceWindow(ksWindow);
    }

    int getWidth()override{return glContext.getWidth();}

    int getHeight()override{return glContext.getHeight();}

    void enableBlending();

    void disableBlending();

public:

    GLuint texProg = 0;

protected:

    GLContext  glContext;

protected:

    virtual bool init() override;

    virtual bool onRender() override;

    friend class KSApplication;

};


#endif //KALASOFT_GLUIRENDERER_H
