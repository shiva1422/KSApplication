//
// Created by Shiva Shanker Pandiri on 5/24/25.
//

#ifndef KALASOFT_APPEVENT_HPP
#define KALASOFT_APPEVENT_HPP


enum AppEventType{

    APP_EVENT_NONE,
    APP_EVENT_MIDI_SERVICE
};

class AppEvent {

public:

    AppEventType eventType = APP_EVENT_NONE;

    int subType = -1;

    void *info1 = nullptr, *info2 = nullptr;

    int intParam1 = -1;
};


class AppEventMonitor {

public:

    virtual void onAppEvent(AppEvent event) = 0;
};


class AppEventProcessor{

public:

    virtual void onAppEvent(AppEvent event) = 0;
};

#endif //KALASOFT_APPEVENT_HPP
