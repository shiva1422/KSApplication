//
// Created by kalasoft on 10/16/21.
//

#ifndef KALASOFT_DISPLAY_H
#define KALASOFT_DISPLAY_H

#include "cstdint"

enum EScreenDirection{X,Y};

class DisplayMetrics{

public:

    int32_t screenWidth=0,screenHeight=0,densityDpi=0,deviceStableDensity=0;

    float density=0.0f,scaledDensity=0.0f,xdpi=0.0f,ydpi=0.0f;

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

    void print();

    void onScreenRotation();

    static void get(DisplayMetrics &dispMetrics);
};

#endif //KALASOFT_DISPLAY_H
