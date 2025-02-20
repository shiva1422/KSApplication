//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 2/15/25.
//

#ifndef KALASOFT_GLSLIDERVIEW_H
#define KALASOFT_GLSLIDERVIEW_H


#include <KSUI/ISliderView.h>
#include "GLImageView.h"

class GLSliderView : public ISliderView{

public:

    GLSliderView();

    ~GLSliderView() override;


    void setValue(const float value) override;


    void setBounds(float startX, float startY, float width, float height) override;

    void setBounds(float width, float height) override;

    void setXY(int x, int y) override;

    void setGradient(float r1, float g1, float b1, float r2, float g2, float b2,
                     float gradientStrength) override;

    void clearBackground() override;

    void draw() override;

    void setPointerImage(const char* path);

    void setBaselineImage(const char*path);

    void setPointerDimensions(float width,float height);

    void setBaseLineHeight(float height);

    void setSliderPosition(float  x);

    ks::Point2DF getPointerCentre(){

        return ks::Point2DF(pointerView.getCentreX(),pointerView.getCentreY());
    }

private:

    GLImageView baseLine,pointerView;
    float pointerWidth = 64,pointerHeigth = 64;
    float baselineHeight = 16;



};


#endif //KALASOFT_GLSLIDERVIEW_H
