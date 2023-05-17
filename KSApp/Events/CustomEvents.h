//
// Created by kalasoft on 12/28/21.
//

#ifndef KALASOFT_CUSTOMEVENTS_H
#define KALASOFT_CUSTOMEVENTS_H

#include <android_native_app_glue.h>

class KSApplication;
class CustomEvents {

public:

    CustomEvents() = delete;

    CustomEvents(KSApplication *app);

    //both below check for events not actually process;
    void processEvent();

    static void customPollProcess(struct android_app* app, struct android_poll_source* source);

    bool addCustomEvent();


private:

    KSApplication *app = nullptr;
    int writeFd,readFd;
    bool bInitialized = false;
    android_poll_source customPollSource;

};


#endif //KALASOFT_CUSTOMEVENTS_H
