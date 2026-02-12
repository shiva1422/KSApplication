//
// Created by kalasoft on 10/9/21.
//

#ifndef KALASOFT_ANDROIDEVENTS_H
#define KALASOFT_ANDROIDEVENTS_H

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "MotionEvent.h"
#include "KeyEvent.h"

//TODO android AGDK has new input type although that renders to surfaceView;from java
class AndroidEvents {

public:

    static int32_t onInputEvent(android_app* papp, AInputEvent* event);

    static void onAppCmd(android_app* app, int32_t cmd);

    static void setMotionEventInterceptor(ks::MotionEventInterceptor *listener){ motionListener = listener;}

    static void setKeyEventInterceptor(ks::KeyEventInterceptor *listener)
    {
        keyEventInterceptor = listener;
        ks::KeyEventInterceptor::mainInterceptor = listener;
    }

private:

   static ks::MotionEventInterceptor *motionListener;
   static ks::KeyEventInterceptor *keyEventInterceptor;

};


#endif //KALASOFT_ANDROIDEVENTS_H
