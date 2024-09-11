//
// Created by Shiva Pandiri on 5/22/2022.
//

#include <Geometry.h>
#include "GLImageView.h"
#include "Display.h"
#include "Shader.h"
GLImageView::GLImageView(const char* path) {

   if(setImage(path))
    if(texture.isValid())
    {
      //  ks::Point2DF centre = View::dispMetrics.getCentre();
      //  GLView::setBounds(centre.x - texture.getWidth()/2, centre.y - texture.getHeight()/2,texture.width,texture.height);
    }
}

GLImageView::~GLImageView() {

}

bool GLImageView::setImage(const char* path)
{
    return texture.setImage(path);
}
void GLImageView::clearBackground() {
    GLView::clearBackground();
}

void GLImageView::setBounds(float startX, float startY, float width, float height) {
    GLView::setBounds(startX, startY, width, height);
}

void GLImageView::draw() {
    //GLView::draw();//TODO

    glEnable(GL_BLEND);
    if(!texture.isValid())
    {
        clearBackground();
        KSLOGW("GLImageView","invalid texture");
        return;
    }
//TODO VAO
    glUseProgram(Shader::getTextureProgram());

    glEnableVertexAttribArray(Shader::getTextureVertsLocation());
    glVertexAttribPointer(Shader::getTextureVertsLocation(), 2, GL_FLOAT, GL_FALSE, 0,(void *)getVertices());

    glEnableVertexAttribArray(Shader::getTextureCoordsLocation());
    glBindBuffer(GL_ARRAY_BUFFER,GLView::defaultUVBufId);
    glVertexAttribPointer(Shader::getTextureCoordsLocation(),2,GL_FLOAT,GL_FALSE,0,(void *) nullptr);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture.tex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,GLView::defaultIndexBufId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    //
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
}


void GLImageView::setBounds(float width, float height) {

    //assert(false);
    //GLView::setBounds(width, height);
}

void GLImageView::setXY(int x, int y) {
    assert(false);
   // View::setXY(x, y);
}

void GLImageView::setGradient(float r1, float g1, float b1, float r2, float g2, float b2,
                              float gradientStrength) {
    View::setGradient(r1, g1, b1, r2, g2, b2, gradientStrength);
}



void GLAlphaBlockImageView::setAlphaNormal(float alpha) {

}

void GLAlphaBlockImageView::setAlphaBounds(float alpha, float startX, float startY, float width,
                                           float height) {

    alphaBlock.setBounds(startX,startY,width,height);
}

void GLAlphaBlockImageView::draw() {

    alphaBounds[0] = 0.0;//startX
    alphaBounds[1] = 1.0;//statY
    alphaBounds[2] = 1.0;//endX
    alphaBounds[3] = 0.0;//endY;

    if(!texture.isValid())
    {
        clearBackground();
        KSLOGW("GLImageView","invalid texture");
        return;
    }
//TODO VAO
    glUseProgram(Shader::getAlphaBlockProgram());

    //TODO improve no need GLView;
    glUniform4f(Shader::getAlphaBlockLocation(),alphaBlock.getStartXGL(),alphaBlock.getStartYGL(),alphaBlock.getEndXGL(),alphaBlock.getEndYGL());

    glEnableVertexAttribArray(Shader::getAlphaBlockVertsLocation());
    glVertexAttribPointer(Shader::getAlphaBlockVertsLocation(), 2, GL_FLOAT, GL_FALSE, 0,(void *)getVertices());

    glEnableVertexAttribArray(Shader::getTextureCoordsLocation());
    glBindBuffer(GL_ARRAY_BUFFER,GLView::defaultUVBufId);
    glVertexAttribPointer(Shader::getAlphaBlockTextCoordsLocation(),2,GL_FLOAT,GL_FALSE,0,(void *) nullptr);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture.getTexture());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,GLView::defaultIndexBufId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    //
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
}
