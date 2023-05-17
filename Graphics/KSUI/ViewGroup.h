//
// Created by kalasoft on 3/5/22.
//

#ifndef KALASOFT_VIEWGROUP_H
#define KALASOFT_VIEWGROUP_H

#include "View.h"

//TODO implement ViewParent and ViewManager also margins,border etc
class ViewGroup : public View{

public:

    ~ViewGroup(){};

    virtual void addView(View * child) = 0;

    //add View at specified offset position  (startXOffset = child->startX - this.getStartX) similar for y
    virtual void addView(View* child,float startXOffset,float starYOffset) = 0;

    virtual int getViewCount() = 0;

    //get View At position inside this ViewGroup
    virtual View* getViewAtPosition(float x,float y) = 0;





};


#endif //KALASOFT_VIEWGROUP_H
