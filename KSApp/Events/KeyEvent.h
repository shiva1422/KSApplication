//
// Created by shivaaz on 12/19/22.
//

#ifndef KALASOFT_KEYEVENT_H
#define KALASOFT_KEYEVENT_H

#include "InputEvent.h"

namespace ks {

    enum EKeyCode{

        KS_KEY_CODE_BACK = AKEYCODE_BACK,
        KS_KEY_CODE_DELETE = AKEYCODE_DEL,
        KS_KEY_CODE_SHIFT_LEFT = AKEYCODE_SHIFT_LEFT,
        KS_KEY_CODE_SHIFT_RIGHT = AKEYCODE_SHIFT_RIGHT,


        KS_KEY_CODE_A = AKEYCODE_A,
        KS_KEY_CODE_B = AKEYCODE_B,
        KS_KEY_CODE_C = AKEYCODE_C,
        KS_KEY_CODE_D = AKEYCODE_D,
        KS_KEY_CODE_E = AKEYCODE_E,
        KS_KEY_CODE_F = AKEYCODE_F,
        KS_KEY_CODE_G = AKEYCODE_G,
        KS_KEY_CODE_H = AKEYCODE_H,
        KS_KEY_CODE_I = AKEYCODE_I,
        KS_KEY_CODE_J = AKEYCODE_J,
        KS_KEY_CODE_K = AKEYCODE_K,
        KS_KEY_CODE_L = AKEYCODE_L,
        KS_KEY_CODE_M = AKEYCODE_M,
        KS_KEY_CODE_N = AKEYCODE_N,
        KS_KEY_CODE_O = AKEYCODE_O,
        KS_KEY_CODE_P = AKEYCODE_P,
        KS_KEY_CODE_Q = AKEYCODE_Q,
        KS_KEY_CODE_R = AKEYCODE_R,
        KS_KEY_CODE_S = AKEYCODE_S,
        KS_KEY_CODE_T = AKEYCODE_T,
        KS_KEY_CODE_U = AKEYCODE_U,
        KS_KEY_CODE_V = AKEYCODE_V,
        KS_KEY_CODE_W = AKEYCODE_W,
        KS_KEY_CODE_X = AKEYCODE_X,
        KS_KEY_CODE_Y = AKEYCODE_Y,
        KS_KEY_CODE_Z = AKEYCODE_Z,

        KS_KEY_CODE_0 = AKEYCODE_0,
        KS_KEY_CODE_1 = AKEYCODE_1,
        KS_KEY_CODE_2 = AKEYCODE_2,
        KS_KEY_CODE_3 = AKEYCODE_3,
        KS_KEY_CODE_4 = AKEYCODE_4,
        KS_KEY_CODE_5 = AKEYCODE_5,
        KS_KEY_CODE_6 = AKEYCODE_6,
        KS_KEY_CODE_7 = AKEYCODE_7,
        KS_KEY_CODE_8 = AKEYCODE_8,
        KS_KEY_CODE_9 = AKEYCODE_9,

    };

    enum EKeyAction{
        KEY_UP = AKEY_EVENT_ACTION_UP,
        KEY_DOWN =AKEY_EVENT_ACTION_DOWN,
    };

    const char AsciiChar[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    class KeyEvent : InputEvent{

    public:

        KeyEvent(AInputEvent *e):ks::InputEvent(e){};

        EKeyCode getKeyCode() const {return static_cast<EKeyCode>(AKeyEvent_getKeyCode(event));}

        char getKeyChar()const{

            //TODO improve
            EKeyCode keyCode = getKeyCode();
           if(isAlphabet())
            {
                return AsciiChar[keyCode-KS_KEY_CODE_A];
            }
            else if(isNumber())
           {
               return keyCode-KS_KEY_CODE_0 + '0';
           }else
               return '~';//TODO
        }
        bool isAlphaNumeric()const
        {
            return isAlphabet() || isNumber();
        }
        bool isAlphabet()const
        {
            EKeyCode keyCode = getKeyCode();
            return (keyCode>= ks::KS_KEY_CODE_A && keyCode <= ks::KS_KEY_CODE_Z);
        }
        bool isNumber()const
        {
            EKeyCode keyCode = getKeyCode();
            return (keyCode>= ks::KS_KEY_CODE_0 && keyCode <= ks::KS_KEY_CODE_9);
        }

        EKeyAction getKeyAction()const
        {
            return static_cast<EKeyAction>(AKeyEvent_getAction(event));
        }

    };

    class KeyEventInterceptor{

    public:

       virtual bool onInterceptKeyEvent(const KeyEvent &keyEvent) = 0;

       virtual void addKeyboardListener(KeyEventInterceptor *l){};

       virtual void removeKeyboardListener(KeyEventInterceptor *l){};

       static KeyEventInterceptor* getMainInterceptor(){return mainInterceptor;}//KSApplication;

       static bool isCapsOn(){return  bIsCapsOn;}


    public:

        static KeyEventInterceptor* mainInterceptor;
        friend class AndroidEvents;
        static bool bIsCapsOn;

    };

    class KeyboardController
    {
    public:

        static bool openKeyboard();

        static bool closeKeyboard();

    private:

        static bool bKeyboardOpen;
    };


}
#endif //KALASOFT_KEYEVENT_H
