//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 1/8/25.
//

#ifndef KALASOFT_HSIIMAGEVIEW_H
#define KALASOFT_HSIIMAGEVIEW_H


#include <GLFrameBuffer.h>
#include "GLImageView.h"

class HSIImageView : public GLImageView{

public:

    HSIImageView();

    virtual ~HSIImageView();

    virtual void draw();

    bool setTextureImage(KSImage *image) override;

    bool setGLTexture(const GLTexture *tex) override;

    bool setTexture(const GLuint texId) override;


public:

    void translateHue(float hueTrans);

    void translateIntensity(float intensity);

    //0 to 100
    void setSaturation(float saturation);

    void enableAutoHueChange(bool  bAuto){bAutoChangeHue = bAuto;}

    //For now separately,later use ImageEditor Meths;

private:

    void updateFrameBuffer();

    void doEditProcess();

private:

    bool bNeedProcess = true;
    bool bAutoChangeHue  = false;

    float hueTranslate = 0;

    float intensityTranslate = 0;

    float saturation = 1.0;

    static int instanceCount;
    static GLuint hsiProgram;
    static GLuint paramsLoc;
    GLFrameBuffer filteredImage;//FOr now not rendering to FrameBuffer but later might use;

public:
    bool setImage(const char *path) override;

};


#endif //KALASOFT_HSIIMAGEVIEW_H
