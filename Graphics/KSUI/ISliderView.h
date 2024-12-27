//
// Created by kalasoft on 2/12/22.
//

#ifndef KALASOFT_ISLIDERVIEW_H
#define KALASOFT_ISLIDERVIEW_H

#include "View.h"

class ISliderView : public View {

    //TODO ONCHangeListeneres;

public:
    //set Value and bounds of pointerView and baselineView accordingly;subclass should also check limits
    virtual void setValue(const float value){this->value = value;};
    float getValue(){return value;}
    float getMinValue(){return minValue;}
    float getMaxValue(){return maxValue;}
    void setRange(const float minVal,const float maxVal){this->maxValue = maxVal,this->minValue = minVal;}

protected:
    //use as value rather that pointer?
    View *pointerView = nullptr;
    View *baselineView = nullptr;

private:
    //add optional Center if needed for non-uniform change before and after centre value
    float value = 0.5f,minValue = 0.0f,maxValue = 1.0f;




};


#endif //KALASOFT_ISLIDERVIEW_H
