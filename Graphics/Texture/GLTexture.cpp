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
    if(glIsTexture(tex))
    glDeleteTextures(1,&tex);


}

bool GLTexture::setImage(const char* path)
{
    KSImage *image = KSImageLoader::loadFromAsset(path);
    bool res  = false;
    if(image)
    {
        res =  setTextureImage(image);
        delete image;
        image = nullptr;
    }

    return res;
}


bool GLTexture::setTextureImage(KSImage *image)
{

    bCreated = true;
    if( !image || !image->isValid())
    {
        //TODO as need now the previous texture remains if existed.
        KSLOGE(TAGLOG,"couldn't create, invalid image");
        bCreated = false;//the previous texture is still hold true
        return bCreated;
    }
    else
    {

        if(( image->width != width || image->height != height))
        {
            KSLOGD(TAGLOG,"Deleting texture");
            if(glIsTexture(tex))
            glDeleteTextures(1,&tex);
            tex = 0;
            width = image->width,height = image->height;
        }
        if(tex == 0)
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
    }

    glBindTexture(GL_TEXTURE_2D,tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
    glBindTexture(GL_TEXTURE_2D,0);
    //delete image; TODO memory leak,Caller/This should coordinate;
    return bCreated;
}

bool GLTexture::setImageFromFile(const char *path) {

    KSImage *image = KSImageLoader::loadFromFile(path);

    bool res  = false;
    if(image)
    {
        res =  setTextureImage(image);
        delete image;
        image = nullptr;
    }
    return res;
}

