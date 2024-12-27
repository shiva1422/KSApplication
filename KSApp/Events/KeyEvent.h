//
// Created by shivaaz on 12/19/22.
//

#ifndef KALASOFT_KEYEVENT_H
#define KALASOFT_KEYEVENT_H

#include "InputEvent.h"

namespace ks {

    enum EKeyCode{

        KS_KEY_CODE_BACK = AKEYCODE_BACK
    };


    class KeyEvent : InputEvent{

    public:

        KeyEvent(AInputEvent *e):ks::InputEvent(e){};

        EKeyCode getKeyCode() const {return static_cast<EKeyCode>(AKeyEvent_getKeyCode(event));}


    };
    class KeyEventInterceptor{

    public:

       virtual bool onInterceptKeyEvent(const KeyEvent &keyEvent) = 0;

    };
}
#endif //KALASOFT_KEYEVENT_H
