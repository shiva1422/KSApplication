//
// Created by Shiva Pandiri on 5/22/2022.
//

#ifndef KALASOFT_GLIMAGEVIEW_H
#define KALASOFT_GLIMAGEVIEW_H


#include <Texture/GLTexture.h>
#include "GLView.h"
#include <array>

class GLImageView : public GLView {

public:

    //TODO check virtual also where to clear image
    GLImageView(){};

     GLImageView(const char* path);

    ~GLImageView() override;

    void clearBackground() override;

    void setBounds(float startX, float startY, float width, float height) override;

    void draw() override;

    bool setImage(const char* path);

    void setBounds(float width, float height) override;

    void setXY(int x, int y) override;

    void setGradient(float r1, float g1, float b1, float r2, float g2, float b2,
                     float gradientStrength) override;

protected:
    //TODO reset as req
    GLTexture texture;

};

/**
 * Same as an Image View , has bounds with a block(one is normal and one bounds to highlight or shadow or inverse)
 * Used to relatively highlight/shadow specific region of the image;
 * Shaders : alphablock.vert,alphablock.frag.
 */
class GLAlphaBlockImageView : public GLImageView{

public:

    void setAlphaNormal(float alpha);

    void setAlphaBounds(float alpha,float startX, float startY,float width,float height);

    float getNormalAlpha(){return normalAlpha;}

    float getBlockAlpha(){return blockAlpha;}

    virtual void draw()override;

private:

    float normalAlpha, blockAlpha;
    std::array<float,4> alphaBounds;
    GLView alphaBlock;

};


#endif //KALASOFT_GLIMAGEVIEW_H
