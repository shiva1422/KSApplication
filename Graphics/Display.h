//
// Created by kalasoft on 10/16/21.
//

#ifndef KALASOFT_DISPLAY_H
#define KALASOFT_DISPLAY_H

#include "cstdint"
#include "Geometry.h"

enum EScreenDirection{X,Y};

class DisplayMetrics{

public:

    int32_t screenWidth=0,screenHeight=0,densityDpi=1,deviceStableDensity=0;

    float density=1.0f,scaledDensity=1.0f,xdpi=1.0f,ydpi=1.0f;

    int pixelToDP(int pixels,EScreenDirection dir)
    {
        if(dir == X)
        {
            return (pixels * densityDpi)/xdpi ;
        }
        else
        {
            return (pixels * densityDpi)/ydpi;
        }
    }

    int DPToPixel(int DP,EScreenDirection dir)
    {
        if(dir == X)
        {
            return (DP * xdpi)/densityDpi;
        }
        else
        {
            return (DP * ydpi)/densityDpi;
        }
    }

    void print() const;

    ks::Point2D<float> getCentre()
    {
        return ks::Point2D<float>(screenWidth/2.0,screenHeight/2.0);
    }


    void onScreenRotation();

    static void get(DisplayMetrics &dispMetrics);

};

#endif //KALASOFT_DISPLAY_H
