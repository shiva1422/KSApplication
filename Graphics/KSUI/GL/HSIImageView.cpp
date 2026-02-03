//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 1/8/25.
//

#include "HSIImageView.h"
#include "Shader.h"
#define LOGTAG "HSIImageView"
HSIImageView::HSIImageView() : GLImageView() {

    if(instanceCount == 0)
    {
        hsiProgram = Shader::createComputeProgram("shaders/common/hsicompute.glsl");
        paramsLoc = glGetUniformLocation(hsiProgram,"params");
    }
    instanceCount++;
    //KSLOGD(TAGLOG,"program %u",hsiProgram);
}

HSIImageView::~HSIImageView() {

    instanceCount--;
    if(instanceCount < 1)
    {
        glDeleteProgram(hsiProgram);
    }
}


void HSIImageView::draw() {

    if(bAutoChangeHue)
    {
        if(hueTranslate >= 350)hueTranslate = 0;//TODO setRate
        translateHue(hueTranslate + 5);
    }
    if(bNeedProcess)
    doEditProcess();

    glUseProgram(Shader::getTextureProgram());

    glEnableVertexAttribArray(Shader::getTextureVertsLocation());
    glVertexAttribPointer(Shader::getTextureVertsLocation(), 2, GL_FLOAT, GL_FALSE, 0,(void *)getVertices());

    glEnableVertexAttribArray(Shader::getTextureCoordsLocation());
    glBindBuffer(GL_ARRAY_BUFFER,GLView::defaultUVBufId);
    glVertexAttribPointer(Shader::getTextureCoordsLocation(),2,GL_FLOAT,GL_FALSE,0,(void *) nullptr);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,filteredImage.getTexId());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,GLView::defaultIndexBufId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    //
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
}


bool HSIImageView::setTextureImage(KSImage *image) {
    bool res =  GLImageView::setTextureImage(image);
    updateFrameBuffer();
    return  res;
}

bool HSIImageView::setGLTexture(const GLTexture *tex) {
    bool res = GLImageView::setGLTexture(tex);
    updateFrameBuffer();
    return res;
}

bool HSIImageView::setTexture(const GLuint texId) {
    bool res = GLImageView::setTexture(texId);
    updateFrameBuffer();
}

void HSIImageView::updateFrameBuffer() {
    filteredImage.setDims(texture.getWidth(),texture.getHeight());
    filteredImage.configure();
}

void HSIImageView::doEditProcess() {

    glUseProgram(hsiProgram);

    glUniform4f(paramsLoc,hueTranslate,saturation,intensityTranslate,1.0);

    glBindImageTexture(0, texture.getTexture(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);
    glBindImageTexture(1,filteredImage.getTexId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);
   // GLContext::getError(TAGLOG);
    glDispatchCompute(texture.getWidth(),texture.getHeight(),1);//TODO divide
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
  //  GLContext::getError(TAGLOG);
    GLContext::getError(LOGTAG);

    bNeedProcess = false;
}

void HSIImageView::translateHue(float hue) {

    if(hue > 360)
    {
        hue = 360;
    }
    hueTranslate = hue;
    bNeedProcess = true;
}

void HSIImageView::translateIntensity(float intensity) {

    intensityTranslate = intensity;
    bNeedProcess = true;
}

void HSIImageView::setSaturation(float psaturation) {
   this->saturation = psaturation;
   bNeedProcess = true;
}

int HSIImageView::instanceCount = 0;
GLuint HSIImageView::hsiProgram = 0;
GLuint HSIImageView::paramsLoc = 0;

bool HSIImageView::setImage(const char *path) {
    bool res =  GLImageView::setImage(path);
    updateFrameBuffer();
    return  res;
}

