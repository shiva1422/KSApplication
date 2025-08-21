//
// Created by kalasoft on 12/24/21.
//

#include <Logger/KSLog.h>
#include <cstring>
#include <cassert>
#include <Shader.h>
#include "GLView.h"
#include "Display.h"
#include<vector>
GLuint GLView::defaultVertexBufId=0;
GLuint GLView::defaultIndexBufId=0;
GLuint GLView::defaultUVBufId=0;
float GLView::defaultVerts[]={-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};


GLView::~GLView() {

}

void GLView::setBounds(float startX, float startY, float width, float height)
{
    View::setBounds(startX, startY, width, height);
    //TODO better Store startX,StartY,widht,height?
    //X's
    vertices[0] = -1.0+ startX * 2.0 /(float)dispMetrics.screenWidth;//*2 so that-1 to 1 else we get 0 to 1 after conversion
    vertices[2] = -1.0+(startX+width)*2.0/(float)dispMetrics.screenWidth;
    vertices[4] = vertices[2];
    vertices[6] = vertices[0];
    //Y's
    vertices[1] = 1.0-(startY+height)*2.0/(float)dispMetrics.screenHeight;
    vertices[3] = vertices[1];
    vertices[5] = 1.0-(startY)*2.0/(float)dispMetrics.screenHeight;
    vertices[7] = vertices[5];
}

void GLView::setBoundsGL(float glsx, float glsy, float glWidth, float glHeight) {

    //X's
    vertices[0] = glsx;//left
    vertices[2] = glsx + glWidth;//right
    vertices[4] = vertices[2];
    vertices[6] = vertices[0];
    //Y's
    vertices[1] = glsy - glHeight;//bottom
    vertices[3] = vertices[1];
    vertices[5] = glsy;//top
    vertices[7] = vertices[5];

    startX = (vertices[0] + 1) * (float)dispMetrics.screenWidth/2.0;
    startY = (- vertices[5] + 1) * dispMetrics.screenHeight/2.0;
    height = glHeight *(float)dispMetrics.screenHeight/2.0;
    width = glWidth *(float)dispMetrics.screenWidth/2.0;
    //TODO super
}

void GLView::setBounds(float width, float height) {
    setBounds(0,0,width, height);
}


void GLView::clearBackground()
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(startX,dispMetrics.screenHeight-startY-height,width,height);//if glScissor enable after this then scissor bounds should bes same as viewport bounds;
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

}




void GLView::draw() {

   //KSLOGI("GLView","DRAW");
 //  if(bVisible)

    glUseProgram(Shader::getRectProgram());
    glEnableVertexAttribArray(Shader::getRectVertsLocation());
    glVertexAttribPointer(Shader::getRectVertsLocation(),2,GL_FLOAT,GL_FALSE,0,(void *)vertices);

    glUniform4f(Shader::getRectColorLocation(),r,g,b,a);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,GLView::defaultIndexBufId);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT, nullptr);

    glDisableVertexAttribArray(Shader::getRectVertsLocation());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

}



float GLView::getStartXGL()const
{
    return vertices[0];
}

float GLView::getStartYGL()const
{
    return vertices[5];
}

float GLView::getEndXGL() const {
    return vertices[4];
}

float GLView::getEndYGL() const {
    return vertices[1];
}


float GLView::getWidthGL()const
{
    return (width*2.0)/(float)dispMetrics.screenWidth;
}

float GLView::getHeightGL()const
{
    return (height*2.0)/(float)dispMetrics.screenHeight;
}

float GLView::getCenterXGL()const {

    return vertices[0] + getWidthGL()/2.0;
}

float GLView::getCenterYGL()const {

    return vertices[5] + getHeightGL()/2.0;
}

bool GLView::initializeUI()
{

    //TODO called only once;
    GLuint bufferIds[3];

    glGenBuffers(3,bufferIds);

    defaultIndexBufId=bufferIds[0], defaultUVBufId=bufferIds[1], defaultVertexBufId=bufferIds[2];
    //for(int i=0;i<3;i++)UILogE("%d, defaultVertexBufId %d",bufferIds[i],defaultVertexBufId);

    glBindBuffer(GL_ARRAY_BUFFER, defaultVertexBufId);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8,(void *)0,GL_STATIC_DRAW);//TODO glBufferSubdata
    float *verts=(float *)glMapBufferRange(GL_ARRAY_BUFFER,0,sizeof(float)*8,GL_MAP_WRITE_BIT);
    if(verts)
    {
        KSLOGI("initialize ui","vets");
        memcpy(verts, defaultVerts, 8 * sizeof(float));

    }
    else
    {
       KSLOGE("initialize ui","vertex upload failed");
        return false;//unamap?
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error


    glBindBuffer(GL_ARRAY_BUFFER, defaultUVBufId);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8,(void *)0,GL_STATIC_DRAW);//TODO glBufferSubData
    float *textCoords=(float *)glMapBufferRange(GL_ARRAY_BUFFER,0,sizeof(float)*8,GL_MAP_WRITE_BIT);
    if(textCoords)
    {

        //0.0f,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f
        //static float textCoods[]={0,1,1,1,1,0,0,0};
        textCoords[0]=0.0f,textCoords[1]=1.0f,textCoords[2]=1.0f,textCoords[3]=1.0f,textCoords[4]=1.0f,textCoords[5]=0.0f,textCoords[6]=0.0f,textCoords[7]=0.0f;

    }
    else
    {////lazy draw on
        ///uploading defaultVerts everydrawcall
       KSLOGE("failed","UIINITIalize()-error uploading textCoods");
        return false;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
    //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, defaultIndexBufId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short )*6, (void *)0, GL_STATIC_DRAW);
    GLushort *indices=(GLushort *)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,0,sizeof(short )*6,GL_MAP_WRITE_BIT);
    if(indices)
    {
        indices[0]=0, indices[1]=1, indices[2]=2, indices[3]=2, indices[4]=3, indices[5]=0;

    }
    else
    {////lazy draw on
        ///uploading defaultVerts everydrawcall
       KSLOGE("failed","UIINITIALIZe()-error uploading indices");
        return false;
    }
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    return true;
}




void GLView::printBounds(const char *tag) {

    KSLOGI(tag, " X  = %f , Y = %f , width = %f, height = %f \n vertices : v0(%f,%f), v1(%f,%f), v2(%f,%f), v3(%f,%f),",startX,startY,width,height,vertices[0],vertices[1],vertices[2],vertices[3],vertices[4],vertices[5],vertices[6],vertices[7]);

}

void GLView::translate(float tx, float ty) {

    vertices[0] += tx;
    vertices[2] += tx;
    vertices[4] += tx;
    vertices[6] += tx;

    vertices[1] += ty;
    vertices[3] += ty;
    vertices[5] += ty;
    vertices[7] += ty;

}

void GLView::setVertices(float *v) {

    for(int i = 0 ; i < 8; ++i)
    {
        vertices[i] = v[i];
    }


}

std::vector<float> GLView::getScreenCoordVerts() {

    std::vector<float> screenVerts(8);

    screenVerts[0] = startX;
    screenVerts[2] = startX+width;
    screenVerts[4] = screenVerts[2];
    screenVerts[6] = screenVerts[0];

    screenVerts[1] = startY + height;
    screenVerts[3] = vertices[1];
    screenVerts[5] = startY;
    screenVerts[7] = vertices[5];

    return screenVerts;
}













