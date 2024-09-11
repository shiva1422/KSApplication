//
// Created by kalasoft on 10/16/21.
//
#include <Logger/KSLog.h>
#include "Display.h"

void DisplayMetrics::print() const {
    KSLOGD("\nDisplayMetrics:","screenWidth %d\nscreenHeight %d\ndensityDpi %d\ndeviceStableDensity %d\ndensity %f\nscaled density %f\nxdpi %f\nydpi %f",screenWidth,screenHeight,densityDpi,deviceStableDensity,density,scaledDensity,xdpi,ydpi);
}
void DisplayMetrics::onScreenRotation()
{
    float temp = screenWidth;
    screenWidth = screenHeight;
    screenHeight = temp;
}
void DisplayMetrics::get(DisplayMetrics &dispMetrics)
{
    //PlatForm

}