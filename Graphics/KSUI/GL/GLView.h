//
// Created by kalasoft on 12/24/21.
//

#ifndef KALASOFT_GLVIEW_H
#define KALASOFT_GLVIEW_H

#include <KSUI/View.h>
#include <Graphics/GLContext.h>
#include <Geometry.h>

class GLView : public View{
    //Just a View storing GLCoordinates
    // call initializeUI() before any GLView operations;//TODO move this to renderer of specific thread
public:

    void clearBackground() override;

    void setBounds(float startX, float startY, float width, float height) override;

    void setBounds(float width, float height) override;

    virtual void draw() override;

    static  bool initializeUI();

    //returns 4 vertices ,be careful size is 8 floats and a pointer
    float* getVertices(){return  vertices;}

    float getStartXGL() const;

    float getStartYGL() const;

    float getWidthGL() const ;

    float getHeightGL() const;

    float getEndXGL() const;

    float getEndYGL() const;

    float getCenterXGL() const;

    float getCenterYGL() const;

    void printBounds(const char* tag);


protected:

    float  vertices[8];//in GL Coordinates

protected:

    //TODO from multiple threads;
    static float  defaultVerts[8];
    static GLuint defaultVertexBufId;
    static GLuint defaultIndexBufId;
    static GLuint defaultUVBufId;


};


#endif //KALASOFT_GLVIEW_H
