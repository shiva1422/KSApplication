//
// Created by Shiva Shanker Pandiri on 3/4/25.
//

#include "GLLineView.h"


GLLineView::~GLLineView() = default;


void GLLineView::draw() {
    GLView::draw();
}


void GLLineView::setBounds(float startX, float startY, float width, float height) {

    GLView::setBounds(startX, startY, width, height);

}

void GLLineView::setStartEndPoints(float startX, float startY, float endX, float endY) {

    //TODO
}

