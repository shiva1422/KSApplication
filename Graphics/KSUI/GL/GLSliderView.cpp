//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 2/15/25.
//



#include "GLSliderView.h"



GLSliderView::GLSliderView() {


}

GLSliderView::~GLSliderView() {

}


void GLSliderView::setValue(const float value) {

    if(value > maxValue)return;
    ISliderView::setValue(value);
    float sx = startX  + width * (value - minValue)/(maxValue - minValue) - pointerView.getWidth()/2.0;
    pointerView.setBounds(sx,pointerView.getStartY(),pointerView.getWidth(),pointerView.getHeight());


}


void GLSliderView::setBounds(float startX, float startY, float width, float height) {
    View::setBounds(startX, startY, width, height);
    baseLine.setBounds(startX,startY,width,baselineHeight);
    pointerView.setBounds(startX + width * (value-minValue)/(maxValue-minValue) - pointerWidth/2.0, baseLine.getCentreY()-pointerHeigth/2.0, height,height);

}

void GLSliderView::setBounds(float width, float height) {
    View::setBounds(width, height);
}

void GLSliderView::setXY(int x, int y) {
    View::setXY(x, y);
}

void GLSliderView::setGradient(float r1, float g1, float b1, float r2, float g2, float b2,
                               float gradientStrength) {
    View::setGradient(r1, g1, b1, r2, g2, b2, gradientStrength);
}

void GLSliderView::clearBackground() {

}

void GLSliderView::draw() {

    baseLine.draw();
    pointerView.draw();

}

void GLSliderView::setPointerImage(const char *path) {

    pointerView.setImage(path);
}

void GLSliderView::setBaselineImage(const char *path) {
    baseLine.setImage(path);
}

void GLSliderView::setPointerDimensions(float width, float height) {


}

void GLSliderView::setBaseLineHeight(float height) {


}

void GLSliderView::setSliderPosition(float x) {

    value = minValue + (maxValue - minValue) * (x - startX)/(getEndX() - getStartX());
    if(value < minValue)value = minValue;
    if(value > maxValue)value = maxValue;
    if(x > getEndX()) x = getEndX();
    if(x < getStartX()) x = getStartX();
    pointerView.setBounds(x - pointerView.getWidth()/2.0, pointerView.getStartY(),pointerView.getWidth(),pointerView.getHeight());
    //TODO update Value;
}

