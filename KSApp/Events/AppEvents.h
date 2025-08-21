//
// Created by kalasoft on 12/28/21.
//

#ifndef KALASOFT_APPEVENTS_H
#define KALASOFT_APPEVENTS_H

#include <android_native_app_glue.h>
#include "AppEvent.hpp"
class KSApplication;


class AppEvents : public AppEventMonitor{

public:

    AppEvents() = delete;

    AppEvents(KSApplication *app);

    //both below check for events not actually process;
    void processEvent();

    static void customPollProcess(struct android_app* app, struct android_poll_source* source);

    void onAppEvent(AppEvent event) override;


    void setEventsProcessor(AppEventProcessor *p)
    {
        this->eventProcessor = p;
    }


private:

    KSApplication *app = nullptr;
    int writeFd,readFd;
    bool bInitialized = false;
    android_poll_source customPollSource;

    AppEventProcessor* eventProcessor = nullptr;

};


#endif //KALASOFT_APPEVENTS_H
