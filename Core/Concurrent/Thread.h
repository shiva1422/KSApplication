//
// Created by shivaaz on 2/9/22.
//

#ifndef KALASOFT_THREAD_H
#define KALASOFT_THREAD_H
#include <Commons.h>
#include <pthread.h>
#include <string>

#define THREADNAMELEN 16
#define TAGLOG "KS::Thread"

typedef void *(*CommonTask_t)(void *);
typedef CommonTask_t ThreadRoutine_t;

enum EThreadType{NORMAL,LOOPER};

namespace ks{

    class Runnable{
    protected:
        virtual void run() = 0;
    };

    //todo destructor
    class Thread{

    //TODO need destruction,start,etc(all oops traits?)?
     //https://android.googlesource.com/platform/dalvik/+/kitkat-release/vm/Thread.cpp#1198
    //Make not copyable but only movable with redirecting Thread.priv;
    //https://www.nextptr.com/tutorial/ta1188594113/passing-cplusplus-captureless-lambda-as-function-pointer-to-c-api
    //https://www.cppstories.com/2019/12/threading-loopers-cpp17/
    public:

        static Thread spawn(ThreadRoutine_t routine, void *opaque = nullptr, const std::string name = "Kala")
        {
            Thread thread;
            //https://docs.oracle.com/cd/E19455-01/806-5257/6je9h032j/index.html
            //TODO merge THreadInof to Thread?
            ThreadInfo *info = new(std::nothrow) ThreadInfo;
            info->name = name;
            info->routine = routine;
            info->opaque = opaque;
            thread.priv = info;
            pthread_create(&thread.mThread, NULL, (void *(*)(void *))(Thread::routine),info);
            info->mThread = thread.mThread;
            //detach default;else Zombies will come out
            return thread;//move/createFail?
        }

        KSFORCEINLINE bool join() const
        {
            void *ret;
            //TODO error check? && check valid uses with ret or delete;
            return !pthread_join(mThread,&ret);
        }

        KSFORCEINLINE bool detach() const
        {
            return !pthread_detach(mThread);
        }

        KSFORCEINLINE bool cancel() const
        {
            //canceltype
            return !pthread_cancel(mThread);
            //to use in Destrucotor?
        }

        KSFORCEINLINE bool setName(const char* name) const
        {
            assert(false);
            //return !pthread_setname_np(mThread,name);TODO
        }

        KSFORCEINLINE std::string getName() const
        {
            char name[THREADNAMELEN] ;
            return (pthread_getname_np(mThread,name,THREADNAMELEN) == 0 ? std::string(name) : std::string(""));
        }


        /*
         * May not be best or fastest for repetitive usage,cache once
         */

        KSFORCEINLINE static std::string getMyName()
        {
            char name[THREADNAMELEN] ={0} ;
            return (pthread_getname_np(pthread_self(),name,THREADNAMELEN) == 0 ? std::string( "KS" + std::string(name)) : std::string("NONAME"));
        }

        KSFORCEINLINE static bool setMyName(const std::string &name)
        {
            //TODO OS SPECIFIC
            return !pthread_setname_np(name.c_str());
        }

    private:

        typedef struct ThreadInfo{

            std::string name ;
            ThreadRoutine_t routine;
            pthread_t mThread;
            void *opaque;

        }ThreadInfo;

        static void cleanup(void *threadInfo)
        {

            KSLog::verbose(TAGLOG,"Cleanup - Exiting Thread : %d name %s",pthread_self(),getMyName().c_str());
            delete threadInfo;
        }

        static void* routine(void *threadInfo)
        {
            ThreadInfo  *info = (ThreadInfo *)(threadInfo);
            std::string name;
            if(info != nullptr)
            {
                name = info->name;
                setMyName(info->name);
                pthread_cleanup_push(cleanup,threadInfo);
                info->routine(info->opaque);
                pthread_cleanup_pop(1);
            }
            else
                KSLog::error(TAGLOG,"error exit on enter- Invalid ThreadInfo");

            KSLog::debug(TAGLOG,"ThreadExit %s",name.c_str());
            pthread_exit(NULL);
            return NULL;//TO invoke cleanup handlers; to continue or cutom threadExit on Phtread_exit();
        }


    protected:
        /*
         * Not TS
         * can invalidate after join or detach or other?
         */
        pthread_t mThread ;
        EThreadType threadType;
        void *priv = nullptr ;
    };

}


/*
 * https://docs.oracle.com/cd/E18752_01/html/816-5137/tlib-1.html#tlib-79569
 * exit,join,cancel,detach,sigpending(2),pthread_sigmask(3),sigaltstack(2),fenv(3),pthread_getcpuclockid(3),sched_setaffinity(2)
 * capabilities - overview of Linux capabilities,pthread_attr_setdetachstate(3),pthread_attr_setstacksize(3),pthread_setattr_default_np(3
 */

#endif //KALASOFT_THREAD_H
