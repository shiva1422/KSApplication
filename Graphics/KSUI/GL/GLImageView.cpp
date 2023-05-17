//
// Created by Shiva Pandiri on 5/22/2022.
//

#include "GLImageView.h"



GLImageView::GLImageView(KSImage image) {
    setImage(image);
}

GLImageView::~GLImageView() {
    //TODO
}

bool GLImageView::setImage(KSImage image)
{
    //TODO recreate
    texture.create(image);
    setBounds(0,0,texture.width,texture.height);
    return texture.tex != 0;
}
void GLImageView::clearBackground() {
    GLView::clearBackground();
}

void GLImageView::setBounds(float startX, float startY, float width, float height) {
    GLView::setBounds(startX, startY, width, height);
}

void GLImageView::draw() {
    GLView::draw();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);

    //draw texture rect
    glBindBuffer(GL_ARRAY_BUFFER,GLView::defaultVertexBufId);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0,(void *)0);
    glBindBuffer(GL_ARRAY_BUFFER,GLView::defaultUVBufId);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,(void *)0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture.tex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,GLView::defaultIndexBufId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,(void *)0);

    //
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
}

void GLImageView::setBounds(float width, float height) {
    View::setBounds(width, height);
}

void GLImageView::setXY(int x, int y) {
    View::setXY(x, y);
}

void GLImageView::setGradient(float r1, float g1, float b1, float r2, float g2, float b2,
                              float gradientStrength) {
    View::setGradient(r1, g1, b1, r2, g2, b2, gradientStrength);
}
