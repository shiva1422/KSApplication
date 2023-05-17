//
// Created by Shiva Pandiri on 5/22/2022.
//

#ifndef KALASOFT_GLIMAGEVIEW_H
#define KALASOFT_GLIMAGEVIEW_H


#include <Texture/GLTexture.h>
#include "GLView.h"


class GLImageView : public GLView {

public:

    //TODO check virtual also where to clear image
    GLImageView(){};
     GLImageView(KSImage image);
    ~GLImageView() override;

    void clearBackground() override;

    void setBounds(float startX, float startY, float width, float height) override;

    void draw() override;

    bool setImage(KSImage image);

    void setBounds(float width, float height) override;

    void setXY(int x, int y) override;

    void setGradient(float r1, float g1, float b1, float r2, float g2, float b2,
                     float gradientStrength) override;

protected:
    //TODO reset as req
    GLTexture texture;

};


#endif //KALASOFT_GLIMAGEVIEW_H
