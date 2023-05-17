//
// Created by Shiva Pandiri on 5/22/2022.
//

#ifndef KALASOFT_GLTEXTURE_H
#define KALASOFT_GLTEXTURE_H

#include <KSMedia/KSFrame//KSImage.h>
#include <KSMedia/KSFrame/KSFrame.h>
#include "../GLContext.h"

class GLTexture{

public:
    GLTexture(){}
    GLTexture(KSImage image);
    void create(KSImage &image);
    void reset(KSFrame *frame,bool bFreeFrame = true);
    ~GLTexture();
    friend class GLImageView;
protected:
    int width = 1,height =1;
    GLuint tex = 0,pbo = 0;
};


#endif //KALASOFT_GLTEXTURE_H
