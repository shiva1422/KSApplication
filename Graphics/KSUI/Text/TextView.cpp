//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 11/24/24.
//

#include <assert.h>
#include "TextView.h"
#include "TextView.h"



//Keep a shared texture map 2dTextureArray for characters used

TextView::TextView() {

  //  textImageView = GLImageView();
}


TextView::~TextView() {

    if(parent != nullptr)
    {
        removeFromParent();//TODO move this to base class
    }
    if(textImage)
        delete textImage;
}


void TextView::clearBackground() {
    GLView::clearBackground();
}

void TextView::setBounds(float startX, float startY, float width, float height) {
    GLView::setBounds(startX, startY, width, height);
    if(textImage != nullptr)//TODO
    {
        //TODO Take Alignment and DP,Crop into consideration;
        float dw = dispMetrics.pixelToDP(textImage->width,EScreenDirection::X);
        float dh = dispMetrics.pixelToDP(textImage->height,EScreenDirection::Y);

        textImageView.setBounds(startX + 16,startY + height/2.0 - dh/2.0,dw,dh);
        if(alignCentre)
        {
            alignTextToCentre();
        }


    }
}

void TextView::setBounds(float width, float height) {
    GLView::setBounds(width, height);
}

void TextView::draw() {

    GLView::draw();
    textImageView.draw();
}

void TextView::setText(const std::string &t) {

    if(text != t)
    {
        //TODO can be improved
        this->text = t;
        if(textImage)
        {
            delete textImage;
        }


        if(TextEngine::getInstance()->getFontSize() != fontSize)
        {
            TextEngine::getInstance()->setTextSize(fontSize,fontSize);
        }
        textImage = TextEngine::getInstance()->getImageFromText(text);

        if(textImage == nullptr)
        {
            assert(false);
        }

        textImageView.setTextureImage(textImage);

        setBounds(startX,startY,width,height);

     //   textImageView.fitToBounds(getStartX(),getStartY(),width,height,true);

    }


}

void TextView::setFontSize(const uint fontSize) {

    this->fontSize = fontSize;

}

void TextView::alignTextToCentre() {

    alignCentre = true;
    float sx = getCentreX() - textImageView.getWidth()/2.0;
    textImageView.setBounds(sx,textImageView.getStartY(),textImageView.getWidth(),textImageView.getHeight());
}

