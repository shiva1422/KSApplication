//
// Created by Shiva Pandiri on 5/20/2022.
//

#ifndef KALASOFT_TEXTURE_H
#define KALASOFT_TEXTURE_H

//TODO interface
#include "GLContext.h"
class Texture {
protected:
    GLuint tex = 0,pbo = 0;
    static GLuint createFromAsset(const char* assetPath);
};


#endif //KALASOFT_TEXTURE_H
