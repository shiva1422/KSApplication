//
// Created by Shiva.Pandiri on 10/6/21.
//

#ifndef KALASOFT_KSWINDOW_H
#define KALASOFT_KSWINDOW_H


#include <android/native_window.h>

class KSWindow{

public:
    ANativeWindow *get()
    {
        return window;
    }
    void setWindow(ANativeWindow *window)
    {
        this->window = window;
    }

    int getWidth();
    int getHeight();

private:
    ANativeWindow *window = nullptr;
};


#endif //KALASOFT_KSWINDOW_H
