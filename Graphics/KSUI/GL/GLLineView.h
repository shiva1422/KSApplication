//
// Created by Shiva Shanker Pandiri on 3/4/25.
//

#ifndef KALASOFT_GLLINEVIEW_H
#define KALASOFT_GLLINEVIEW_H

#include "GLView.h"

class GLLineView : public GLView{
public:
    void draw() override;

    void setBounds(float startX, float startY, float width, float height) override;

    void setStartEndPoints(float startX,float startY,float endX,float endY);//TODO move this to view,GLView;

    ~GLLineView() override;

};


#endif //KALASOFT_GLLINEVIEW_H
