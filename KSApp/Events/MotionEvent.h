//
// Created by shivaaz on 12/19/22.
//

#ifndef KALSOFT_MOTIONEVENT_H
#define KALSOFT_MOTIONEVENT_H

#include <unordered_map>
#include "android/input.h"
#include "InputEvent.h"

#include "stdint.h"

#define TOUCHID_INVALID INT32_MIN

namespace ks{

    typedef int32_t TouchID;



    enum EMotionEventAction{

        DOWN         = AMOTION_EVENT_ACTION_DOWN,
        POINTER_DOWN = AMOTION_EVENT_ACTION_POINTER_DOWN,
        MOVE         = AMOTION_EVENT_ACTION_MOVE,
        POINTER_UP   = AMOTION_EVENT_ACTION_POINTER_UP,
        UP           = AMOTION_EVENT_ACTION_UP,
        CANCEL       = AMOTION_EVENT_ACTION_CANCEL,

        HOVER_ENTER  = AMOTION_EVENT_ACTION_HOVER_ENTER,
        HOVER_EXIT   = AMOTION_EVENT_ACTION_HOVER_EXIT,
        HOVER_MOVE   = AMOTION_EVENT_ACTION_HOVER_MOVE,

        OUTSIDE      = AMOTION_EVENT_ACTION_OUTSIDE,
        SCROLL       = AMOTION_EVENT_ACTION_SCROLL,

        BUTTON_PRESS = AMOTION_EVENT_ACTION_BUTTON_PRESS,
        BUTTON_RELEASE = AMOTION_EVENT_ACTION_BUTTON_RELEASE

    };

    class MotionEvent : public InputEvent{

    public:

        //TODO inside class def methods already inlined
        MotionEvent(AInputEvent *e):ks::InputEvent(e){};

        KSFORCEINLINE EMotionEventAction getAction()const{return static_cast<EMotionEventAction>(AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK);}

        //each finger touched or mouse active has stored index,except move other actions(pointer up/down) are triggered with this index as current in the event
        KSFORCEINLINE int32_t getPointerIndex() const{return (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;}

        //unlike pointerIndex pointerId is unque for a finger/mouse until its active,where as index changes every  touch/mouse action
        KSFORCEINLINE int32_t getPointerId()const{return AMotionEvent_getPointerId(event,getPointerIndex());}

        KSFORCEINLINE int32_t getPointerId(int32_t index) const{return AMotionEvent_getPointerId(event,index);}

        KSFORCEINLINE float getX(int32_t pointerIndex)const{return AMotionEvent_getX(event, pointerIndex);}

        KSFORCEINLINE float getY(int32_t pointerIndex)const{return AMotionEvent_getY(event, pointerIndex);}

        KSFORCEINLINE int getPointerCount()const{return AMotionEvent_getPointerCount(event);}


    };


    class MotionEventInterceptor{

    public:

        virtual bool onInterceptMotionEvent(const MotionEvent& me) = 0;

    };


}


#endif //KALASOFT_MOTIONEVENT_H
