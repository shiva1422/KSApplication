//
// Created by kalasoft on 12/28/21.
//

#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <Logger/KSLog.h>
#include "CustomEvents.h"
#include "KSApp/KSApplication.h"




CustomEvents::CustomEvents(KSApplication *app) {

    this->app = app;
    int msgpipe[2];
    if (pipe(msgpipe)) {
        KSLog::error("Custom looper events","could not create pipe: %s", strerror(errno));
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
    KSLog::verbose("CustomEvent init","success");
    bInitialized = true;
}


void CustomEvents::customPollProcess(struct android_app* app, struct android_poll_source* source)
{
    KSLog::verbose("Custom poll pRoces","input recieved on Custom Event");
    CustomEvents  *eventProcessor = (static_cast<KSApplication *>(app->userData))->customEvents;
    if(eventProcessor)
    {
            eventProcessor->processEvent();
    }

}

void CustomEvents::processEvent()
{

 /*   CustomMsg msg;
    if (read(readFd, &msg, sizeof(msg)) == sizeof(msg))
    {
        switch (msg.eEventType)
        {
            case IMPORTIMAGE:
            {
                PhotoApp *globalData= static_cast<PhotoApp *>(app->userData);
                globalData->onImportImage(msg.fd);
            }break;

        }
    }
    else
    {
        Loge("CustomPollProcess","cannot obtain msg");
    }*/
}

