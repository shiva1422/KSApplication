//
// Created by shivaaz on 9/10/22.
//
#include <unistd.h>
#include <cstdio>
#include <cassert>
#include "../Thread.h"

using namespace kala;
void *therad1(void *data)
{
    int i = 0;
    while(i < 100)
    {

        KSPRINTF("\n Thead = %s - %d\n",Thread::getMyName().c_str(),i++);
       usleep(100000);
    }
}

void  *thread2(void *data)
{
    int i = 0;
    while(i < 150)
    {
        KSPRINTF("\nThread = %s \n",Thread::getMyName().c_str(),i++);
        usleep(100000);
    }
}



int main (int argc, char *argv[])
{

   // ThreadRoutine_t routine = thread2;
   // routine(NULL);
    setbuf(stdout, 0);//CUZ log not showing
    KSPRINTF("hello");

    Thread t1;
    t1 = ks::Thread::spawn(therad1,NULL,std::string( "kala" + std::to_string(0) ));
       // Thread t2 = kala::Thread::spawn(thread2);
    usleep(1000000);
    //t1.detach();
    t1.cancel();
    //t1.join();
  //  t2.cancel();

    pthread_exit(NULL);//so other threads continue
}