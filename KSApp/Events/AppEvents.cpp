//
// Created by kalasoft on 12/28/21.
//

#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <Logger/KSLog.h>
#include "AppEvents.h"
#include <android/looper.h>
#include "KSApp/KSApplication.h"
#include "AppEvent.hpp"

#define LOGTAG "AppEvents"
AppEvents::AppEvents(KSApplication *app) {

    this->app = app;
    int msgpipe[2];
    if (pipe(msgpipe)) {
        KSLOGE("Custom looper events","could not create pipe: %s", strerror(errno));
        return ;
    }
    readFd = msgpipe[0];
    writeFd = msgpipe[1];
    customPollSource.id = LOOPER_ID_USER;
    customPollSource.app = app->app;
    customPollSource.process = customPollProcess;
    readFd = msgpipe[0];
    writeFd = msgpipe[1];
    ALooper_addFd(app->app->looper, readFd, LOOPER_ID_USER, ALOOPER_EVENT_INPUT, nullptr,&customPollSource);
    KSLOGV("CustomEvent init","success");
    bInitialized = true;
}


void AppEvents::customPollProcess(struct android_app* app, struct android_poll_source* source)
{
    KSLOGV("Custom poll pRoces","input recieved on Custom Event");
    AppEvents  *eventProcessor = (static_cast<KSApplication *>(app->userData))->appEvents;
    if(eventProcessor)
    {
            eventProcessor->processEvent();
    }

}

void AppEvents::processEvent()
{
    AppEvent event;

    if (read(readFd, &event, sizeof(event)) == sizeof(event))
    {

        if(eventProcessor)
        {
            eventProcessor->onAppEvent(event);
        }

    }
    else
    {
        KSLOGE(LOGTAG,"cannot obtain event");
    }


}

void AppEvents::onAppEvent(AppEvent event) {

    //TODO multiple events;
    if(write(writeFd, &event, sizeof(event)) != sizeof(event))
    {
        KSLOGE("addCustomEvent","Failure writing android_app cmd: %s\n", strerror(errno));
        return;
    }

    KSLOGD("CustomEvent","added");
}

