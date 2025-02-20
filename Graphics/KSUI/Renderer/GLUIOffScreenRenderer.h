//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 1/5/25.
//

#ifndef KALASOFT_GLUIOFFSCREENRENDERER_H
#define KALASOFT_GLUIOFFSCREENRENDERER_H


#include "GLRenderer.h"

class GLUIOffScreenRenderer : public GLRenderer{


public:

    bool init(int width,int height);

    bool onRender() override;

};


#endif //KALASOFT_GLUIOFFSCREENRENDERER_H
