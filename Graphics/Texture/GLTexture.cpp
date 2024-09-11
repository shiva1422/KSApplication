//
// Created by Shiva Pandiri on 5/22/2022.
//

#include "string"
#include <Logger/KSLog.h>
#include <string>
#include <Media/KSImage/KSImageLoader.h>
#include "GLTexture.h"

#define TAGLOG "GLTexture"

GLTexture::GLTexture(const char* path)
{
    setImage(path);
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1,&tex);
    tex = 0;
}

bool GLTexture::setImage(const char* path)
{
    KSImage *image = KSImageLoader::loadFromAsset(path);
    bCreated = true;
    if( !image || !image->isValid())
    {
        //TODO as need now the previous texture remains if existed.
        KSLOGE(TAGLOG,"couldn't create, invalid image");
        bCreated = false;//the previous texture is still hold true
        return bCreated;
    }
    if(glIsTexture(tex) && ( image->width != width || image->height != height))//TODO format
        glDeleteTextures(1,&tex);
    else
    {
        width = image->width; height = image->height;
        glGenTextures(1,&tex);
        glBindTexture(GL_TEXTURE_2D,tex);
        //TODO.GL_NEAREST is faster
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
        glBindTexture(GL_TEXTURE_2D,tex);
        bCreated = true;
    }

    glBindTexture(GL_TEXTURE_2D,tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
    glBindTexture(GL_TEXTURE_2D,0);
    //delete image; TODO memory leak
    return bCreated;
}

