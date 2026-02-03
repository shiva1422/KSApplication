//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 1/4/25.
//

#ifndef KALASOFT_GLFRAMEBUFFER_H
#define KALASOFT_GLFRAMEBUFFER_H


#include "GLContext.h"
#include "Geometry.h"
#include "../../../android/SequensaPhoto/app/src/main/jni/Apps/PhotoAndVideo/PhotoEditor/Texture/TextureID.hpp"


class GLFrameBuffer {


public:

    GLFrameBuffer();//by default creates screenSized buffer for other size use other Construtors.

    GLFrameBuffer(int width,int height);

    virtual ~GLFrameBuffer();

    void configure();

    GLuint getId()const{return id;}//combine to make return all ids using one function.

    GLuint getTexId()const{return texId;}

    void setDims(int width,int height);

    /**
     * @brief Binds the default screen/pBuffer;
     *
     * @param width, height : width and height of the the default frame buffer, if less than 0 then set to screen/display Size
     */
    static void setToDefault(int widht = -1,int height = -1);

    void reconfigure(int width,int height);

    void makeActive();

    GLuint getBufferTexture(){return texId;}

    int getWidth(){return width;}

    int getHeight(){return  height;}

    /**
     * @brief once this function returns the texture and therefore the frameBuffer becomes invalid. reconfigure if a reuse need or delete;
     * @return the texture attached to FrameBuffer,
     */
    Texture aquireTexture();


    ks::Size<int> getSize(){return ks::Size<int>(width,height);}

    void rebindTexture();

private:

    void configureColorBuffer();//configure should be done after these two funcs

    void configureDepthBuffer();

     void clearResources();


protected:

    GLuint id = 0;

    GLuint texId = 0;

    GLuint depthBufId = 0;

    int width = 0,height = 0;


};


#endif //KALASOFT_GLFRAMEBUFFER_H
