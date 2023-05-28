//
// Created by shivaaz on 2/15/22.
//

#ifndef KALASOFT_LOOPERTHREAD_H
#define KALASOFT_LOOPERTHREAD_H

#include "Thread.h"
namespace ks{

    class LooperThread {

       Thread thread;

    public:

         Thread* getThreadInstance(){return &thread;}
    };

}


#endif //KALASOFT_LOOPERTHREAD_H
