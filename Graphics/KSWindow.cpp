//
// Created by kalasoft on 10/6/21.
//

#include "KSWindow.h"

int KSWindow::getWidth()
{
    //if null ??
    if(window)
    return ANativeWindow_getHeight(window);
    return -1;
}

int KSWindow::getHeight()
{
    if(window)
        return ANativeWindow_getWidth(window);
    return -1;
}