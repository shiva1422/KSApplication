//
// Created by kalasoft on 12/23/21.
//

#ifndef KALASOFT_RENDERER_H
#define KALASOFT_RENDERER_H

#include <Graphics/Display.h>
#include "Logger/KSLog.h"
class Renderer {

public:
    static void setDisplayMetrics(DisplayMetrics displayMetrics);

    void setContent(void *view)
    {
        KSLOGD("renderer","set Content");
        this->content = view;
    }

    void *getContent() const
    {
        KSLOGD("renderer","set Content");
        return  this->content;
    }

    void setBackGroundColor(float r,float g,float b,float a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    friend class KSApplication;

protected:

    virtual bool onRender() = 0;

    virtual bool init() = 0;


protected:

    void *content = nullptr;

    float r = 0.0,g=0.0,b=0.0,a=1.0;


private:


};

#endif //KALASOFT_RENDERER_H
