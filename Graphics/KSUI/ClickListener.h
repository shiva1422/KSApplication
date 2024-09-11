//
// Created by shivaaz on 9/2/24.
//

#ifndef KALASOFT_CLICKLISTENER_H
#define KALASOFT_CLICKLISTENER_H

#include "View.h"
#include "Geometry.h"

class ClickListener : private View::TouchListener {


protected:

    virtual ~ClickListener();

    //This should also be considered as onClickUp;
    virtual bool onClick() = 0;

    virtual bool onClickDown(){return false;}


private:


    virtual bool onTouch(const ks::MotionEvent &event, View *view)override;

    virtual bool
    onTouchDown(const float &x, const float &y, const ks::TouchID &id, const bool &isPrimary)override;

    virtual bool
    onTouchUp(const float &x, const float &y, const ks::TouchID &id, const bool &isLast)override;

    virtual bool onMove(const float &x, const float &y, const ks::TouchID &id)override;

    virtual bool onHoverExit(const ks::TouchID &id, const float &x, const float &y) override;

    virtual bool onHoverEnter(const float &d, const float &d1, const ks::TouchID &i)override;


protected:

    using View::TouchListener::view;

private:

    float initialTouchX, initialTouchY;

    float yDiffAllowed = 10, xDiffAllowed = 10;//finger can move 10 * 10 pixel still to be consider Click;

    bool clickActive = false;

    bool isMultipleClickEnabled = false;

    std::unordered_map<ks::TouchID,ks::Point2DF> clickPointers;


    ks::TouchID clickId = INT_MIN;


    //friend class TouchListener;
    friend class View;

};


#endif //KALASOFT_CLICKLISTENER_H
