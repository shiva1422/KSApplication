  //
// Created by kalasoft on 10/9/21.
//

#include <android_native_app_glue.h>
#include "AndroidEvents.h"
#include <Logger/KSLog.h>
#include "InputEvent.h"
#include "MotionEvent.h"
#define TAGLOG "INPUT EVENT"


//TODO return type
int32_t AndroidEvents::onInputEvent(android_app* papp, AInputEvent* event)
{

    ks::InputEvent eventWrap = event;

    if(eventWrap.getEventType() == ks::EInputEventType::MOTION)
    {
        ks::MotionEvent me = event;
        return  motionListener->onInterceptMotionEvent(me);
    }
    else if(eventWrap.getEventType() == ks::EInputEventType::KEY)
    {
        KSLOGD(TAGLOG,"KeyEvent");
        return keyEventInterceptor->onInterceptKeyEvent(event);
    }
    else if(eventWrap.getEventType() == ks::EInputEventType::FOCUS)
    {
        //TODO
        //assert(false);
        KSLOGE("AndroidEvent","unhandled");
    }
    else
    {
        KSLOGE("KSEVENT","UNKOWN TYPE");
       // assert(false);
    }

    return 0;
}

//TODO make independent of application
#include "KSApp/KSApplication.h"
void AndroidEvents::onAppCmd(android_app* app, int32_t cmd)
{
    KSApplication *application = static_cast<KSApplication *>(app->userData);
    switch(cmd)
    {
        /*java Lauch(onCreate(ANativeActivity_OnCreate->android_main) ->Created(Onstart)->Started follows below in the same ordered
         * better only render(incluing audio) only after all resumed,focus gained and windowCreated.
         * */
        case APP_CMD_START:
            application->onStart();
            break;
        case APP_CMD_RESUME:
            //app in foreground
            application->onResume();
            break;
        case APP_CMD_PAUSE:
            application->onPause();
            break;
        case APP_CMD_STOP:
            application->onStop();
            break;
        case APP_CMD_DESTROY:
            application->onDestroy();
            //destroy opengl;
            break;
        case APP_CMD_LOW_MEMORY:
            application->onLowMemory();
            break;
        case APP_CMD_WINDOW_RESIZED:
            application->onWindowResized();
            break;
        case APP_CMD_SAVE_STATE:
            // the OS asked us to save the state of the app
            application->onSaveState();
            break;
        case APP_CMD_INIT_WINDOW:
        {
            application->setWindow(app->window);
            application->onWindowInit();

        }
            break;
        case APP_CMD_TERM_WINDOW:
        {
            application->onWindowTermination();
        }
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            application->onWindowRedrawNeeded();
            break;
        case APP_CMD_INPUT_CHANGED:
            application->onInputChange();
            break;

        case APP_CMD_CONTENT_RECT_CHANGED:
            application->onContentRectChanged();
            break;
        case APP_CMD_CONFIG_CHANGED:
        {
            application->onConfigChanged();
        }break;

        case APP_CMD_LOST_FOCUS:
            application->onFocusLost();
            //drawggin notification drawer sitching away to another app,(can happen without being paused)
            // if the app lost focus, avoid unnecessary processing
            //(like monitoring the accelerometer)inputs etc//(in games should be paused as control is lost).
            break;
        case APP_CMD_GAINED_FOCUS:
            application->onFocusGained();
            // bring back a certain functionality, like monitoring the accelerometer
            break;
        default:
            KSLOGE("UnknownCommand","onAppCMD");
    }
    KSLOGI("command handling","done");

}

ks::MotionEventInterceptor *AndroidEvents::motionListener = nullptr;//KSApplication
ks::KeyEventInterceptor *AndroidEvents::keyEventInterceptor = nullptr;//KSApplication;
ks::KeyEventInterceptor *ks::KeyEventInterceptor::mainInterceptor = nullptr;
bool ks::KeyEventInterceptor::bIsCapsOn = false;