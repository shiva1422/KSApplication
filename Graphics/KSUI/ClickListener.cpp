//
// Created by shivaaz on 9/2/24.
//

#include "ClickListener.h"
#include "View.h"

ClickListener::~ClickListener() {

}

bool ClickListener::onTouch(const ks::MotionEvent &event, View *view) {

    return false;
}

bool ClickListener::onTouchDown(const float &x, const float &y, const ks::TouchID &id,
                                const bool &isPrimary) {

    if(clickPointers.size() > 0 && !isMultipleClickEnabled)
    { //Since only handling single click we do not monitor further touches.
        KSLOGD("KSEVENT", "Click Warning");
        return false;
    }

    KSLOGD("KSEVENT", "Click Down");

    clickPointers.insert({id, {x, y}});
    clickX = x;
    clickY = y;
    onClickDown();
    return true;

}

bool ClickListener::onTouchUp(const float &x, const float &y, const ks::TouchID &id,
                              const bool &isLast) {

    if(clickPointers.find(id) != clickPointers.end())
    {
        auto point = clickPointers.find(id)->second;
        if(fabs(point.y - y) < yDiffAllowed && fabs(point.x - x) < xDiffAllowed)
        {
            clickX = x;
            clickY = y;
            onClick();
            KSLOGD("KSEVENT", "ClickUp");

        }else
            onClickCancel();
        clickPointers.erase(id);
    }else
    {
        KSLOGW("Click","pointer doesn't exist");

    }
    if(isLast)
        clickPointers.clear();

    KSLOGW("Click","pointer exit");


    return false;
}

bool ClickListener::onMove(const float &x, const float &y, const ks::TouchID &id) {

    return true;
}

bool ClickListener::onHoverExit(const ks::TouchID &id, const float &x, const float &y) {

    KSLOGD("Click","Hover Exit at point (%f , %f), bounds  %f %f %f %f",x,y,view->getStartX(),view->getStartY(),view->getEndX(),view->getEndY());
    //TODO if renter should allow,can be handled here.
    clickPointers.erase(id);
    return TouchListener::onHoverExit(id, x, y);
}

bool ClickListener::onHoverEnter(const float &d, const float &d1, const ks::TouchID &i) {
    return TouchListener::onHoverEnter(d, d1, i);
}
