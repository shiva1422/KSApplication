//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 11/22/24.
//

#ifndef KALASOFT_IANIMATOR_H
#define KALASOFT_IANIMATOR_H


#include "../Geometry.h"
#include<chrono>
#include <ctime>


/*
 * Animation need not be limited to Position(Scale,Translate, Rotate). It shall be extended to anything that needs/can be changed(Interpolated) with time for example opacity. And such thing will produce interesting transitions in the Scene.
 * can extend to brightness,saturation and what not.
 */

typedef enum EAnimationType{
  ANIM_NONE,
}EAnimType;


typedef enum EAnimationInterpolationType{
    LINEAR,
    SINUSOID

}EAnimInterpolation;


typedef struct AnimationParameters{

    ks::Point3DF posStart, posEnd;
    ks::Point3DF scaleStart, scaleEnd;
    ks::Point3DF rotStart, rotEnd;

}AnimParams;

//TODO perf check if custom doing on cpu is better than using MVP Matrices;
class IAnimator {

public:

   virtual void setAnimationControls(EAnimationType pAnimType, float animSecs, EAnimInterpolation interpolation = LINEAR, unsigned int pRepeat = 1)
   {
       bAnimate = (animSecs != 0);
       this->animType = pAnimType;
       animDuration = animSecs;
       this->interPol = interpolation;
       this->repeatCount = pRepeat;
       bAnimating = false;
   }

   virtual void setAnimParams(const AnimParams params){this->params = params;}

   virtual void onAnimationStart()
   {
       bAnimating = true;
       animStartTime  = std::chrono::system_clock::now();
   }
   virtual void onAnimationEnd(){}





protected:

    bool bAnimate = false,bAnimating = false;
    float animDuration;
    unsigned int repeatCount;
    EAnimationType animType;
    EAnimInterpolation interPol;
    AnimParams params;
    std::chrono::time_point<std::chrono::system_clock> animStartTime;


};


#endif //KALASOFT_IANIMATOR_H
