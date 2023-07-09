//
// Created by kalasoft on 10/16/21.
//

#include <Display.h>
#include <cstring>
#include <Logger/KSLog.h>
#include "View.h"


DisplayMetrics View::dispMetrics = {0};
/*
 * Check centreX , centre Y without bounds
 */
View::View(){}

View::~View()
{
    delete touchListener;
    touchListener = nullptr;
}

View::View(float width, float height)
{
    setBounds(0,0,width,height);
}

View::View(float startX, float startY, float width, float height)
{

    setBounds(startX,startY,width,height);
}

void View::setBounds(float startX, float startY, float width, float height)
{

    //KSLOGD("setBounds","%d,%d,%d,%d",startX,startY,width,height);
    this->startX = startX,this->startY=startY,this->width=width,this->height=height;

}

void View::setBounds(float width, float height)
{
    setBounds(0,0,width,height);
}
void View::setBackgroundColor(float red, float green, float blue, float alpha)
{
    r = red;
    g = green;
    b = blue;
    a = alpha;
}

void View::fitToBounds(int BStartX, int BStartY, int BWidth, int BHeight,bool keepAspectRation )
{
    //TODO aspect ratio to float and denoms zeores
    float newWidth,newHeight;
    //float aspectRatioImage=width/height;
    float xRation,yRation;
    xRation=width/BWidth;
    yRation=height/BHeight;
    if(xRation>1 || yRation>1)
    {
        float finalRatio;
        if(xRation>=yRation)
            finalRatio=xRation;
        else
            finalRatio=yRation;

        newHeight=height/finalRatio;
        newWidth=width/finalRatio;

    }
    else
    {
        newWidth=width;
        newHeight=height;
    }
    float  xdiff=BWidth-newWidth;
    float  ydiff=BHeight-newHeight;
    //  {UILogE("negative difference xdiff %f and ydiff %f",xdiff,ydiff);}
    setBounds(BStartX+(xdiff/2.0),BStartY+(ydiff/2.0),newWidth,newHeight);
}
void View::fitToBoundsWithCentre(int centreX, int centreY, int BWidth, int BHeight)
{
    fitToBounds(centreX - BWidth/2.0,centreY - BHeight/2.0,BWidth,BHeight);
}


/*void View::onScreenRotation()
{
    //Display Metric should also be rotated x<->y
    KSLOGI("view screenRot","%f %f %f %f",startX,startY,width,height);
     setBounds(startY,startX,width,height);
}*/


