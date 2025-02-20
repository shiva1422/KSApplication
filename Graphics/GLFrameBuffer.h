//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 1/4/25.
//

#ifndef KALASOFT_GLFRAMEBUFFER_H
#define KALASOFT_GLFRAMEBUFFER_H


#include "GLContext.h"

class GLFrameBuffer {


public:

    GLFrameBuffer();//by default creates screenSized buffer for other size use other Construtors.

    GLFrameBuffer(int width,int height);

    ~GLFrameBuffer();

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

    void makeActive();

    GLuint getBufferTexture(){return texId;}


private:

    void configureColorBuffer();//configure should be done after these two funcs

    void configureDepthBuffer();

private:

    GLuint id = 0;

    GLuint texId = 0;

    GLuint depthBufId = 0;

    int width = 0,height = 0;


};


#endif //KALASOFT_GLFRAMEBUFFER_H
