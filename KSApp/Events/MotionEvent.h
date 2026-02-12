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

class GameActivityMotionEvent;

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


   // https://github.com/android/games-samples/blob/8ad152d7e571bb4d43136d6d6acc7a8cb4bf2cc6/agdk/game_controller/app/src/main/cpp/native_engine.cpp#L148
   // https://developer.android.com/games/agdk/game-activity/migrate-native-activity
   // https://developer.android.com/games/agdk/game-activity/get-started


    class MotionEvent {

    public:


        MotionEvent() = delete;

        MotionEvent(GameActivityMotionEvent* me);


        KSFORCEINLINE EMotionEventAction getAction()const{return action;}

        //each finger touched or mouse active has stored index,except move other actions(pointer up/down) are triggered with this index as current in the event
      //  KSFORCEINLINE int32_t getPointerIndex() const{return }

        //unlike pointerIndex pointerId is unque for a finger/mouse until its active,where as index changes every  touch/mouse action
        KSFORCEINLINE int32_t getPointerId()const{return pointerId;}//TODO can cache index check

       // KSFORCEINLINE int32_t getPointerId(int32_t index) const{return }

        KSFORCEINLINE float getX()const{
           // return GameActivityPointerAxes_getX(&event->pointers[getPointerIndex()]);
            return x;
        }//TODO can cache pointer index

        KSFORCEINLINE float getY()const{
           // return GameActivityPointerAxes_getY(&event->pointers[getPointerIndex()]);
            return y;
        }




    private:

        GameActivityMotionEvent *event = nullptr;

        EMotionEventAction action;

        float x, y;

        int32_t pointerId = TOUCHID_INVALID;


        //TODO add Motion Source /Touchscreen/mouse


    };


    class MotionEventInterceptor{

    public:

        virtual bool onInterceptMotionEvent(const MotionEvent& me) = 0;

    };


}


#endif //KALASOFT_MOTIONEVENT_H
