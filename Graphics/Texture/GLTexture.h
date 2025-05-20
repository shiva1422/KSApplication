//
// Created by Shiva Pandiri on 5/22/2022.
//

#ifndef KALASOFT_GLTEXTURE_H
#define KALASOFT_GLTEXTURE_H

#include <CMedia/KSImage.h>
#include <CMedia/KSFrame.h>
#include "../GLContext.h"

class GLTexture{

public:

    GLTexture(){}

    ~GLTexture();

    GLTexture(const char* path);

    bool setImage(const char *path);

    bool setImageFromFile(const char* path);

    bool setTextureImage(KSImage *image);


    int getWidth()const{return this->width;}

    int getHeight()const{return this->height;}

    GLuint getTexture()const{return this->tex;}



    /**
     * this doesn't handle all the cases yet
     * @return
     */
    bool isValid()const{return tex!=0;}

    friend class GLImageView;

    GLuint tex = 0;

protected:

    int width = -1,height = -1;

    bool bCreated = false;
};


#endif //KALASOFT_GLTEXTURE_H
