//
// Created by shivaaz on 12/19/22.
//

#ifndef KALASOFT_INPUTEVENT_H
#define KALASOFT_INPUTEVENT_H

#include <Commons.h>
#include "android/input.h"

namespace ks
{

    //TODO later modify for cross platform
    enum EInputEventType{

        /** Touch or Mouse*/
        MOTION = AINPUT_EVENT_TYPE_MOTION,

        /** Keyboard */
        KEY    = AINPUT_EVENT_TYPE_KEY,

        FOCUS  = AINPUT_EVENT_TYPE_FOCUS,//TODO handle in app

    };

    class InputEvent{

    public:

        InputEvent() = delete;

        InputEvent(AInputEvent *pEvent):event(pEvent){}

        KSFORCEINLINE EInputEventType getEventType()const{return static_cast<EInputEventType>(AInputEvent_getType(event));}



    protected:

        const AInputEvent *event;
    };

}


#endif //KALASOFT_INPUTEVENT_H
