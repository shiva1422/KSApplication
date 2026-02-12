//
// Created by shivaaz on 12/19/22.
//

#include "MotionEvent.h"

#include <game-activity/GameActivityEvents.h>

ks::MotionEvent::MotionEvent(GameActivityMotionEvent *me) {

    this->event = me;
    action = static_cast<EMotionEventAction>(event->action & AMOTION_EVENT_ACTION_MASK);

    auto pointerIndex = (event->action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    pointerId = event->pointers[pointerIndex].id;

    x = GameActivityPointerAxes_getX(&event->pointers[pointerIndex]);
    y = GameActivityPointerAxes_getY(&event->pointers[pointerIndex]);
}
