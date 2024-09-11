//
// Created by kalasoft on 9/30/21.
//

#ifndef KALASOFT_KSLOG_H
#define KALASOFT_KSLOG_H

#include <android/log.h>

//first three are typically disabled in release builds.
#define KSLOGV(...)((void)__android_log_print(ANDROID_LOG_VERBOSE,__VA_ARGS__))
#define KSLOGD(...)((void)__android_log_print(ANDROID_LOG_DEBUG,__VA_ARGS__))
#define KSLOGI(...)((void)__android_log_print(ANDROID_LOG_INFO,__VA_ARGS__))

#define KSLOGW(...)((void)__android_log_print(ANDROID_LOG_WARN,__VA_ARGS__))
#define KSLOGE(...)((void)__android_log_print(ANDROID_LOG_ERROR,__VA_ARGS__))
//when aborting.
#define KSLOGF(...)((void)__android_log_print(ANDROID_LOG_FATAL,__VA_ARGS__))

/*Dont use below for logging better use above macros*/

/*
class KSLog {

public:
    static int error(const char *tag,const char* format,...);
    static int info(const char *tag,const char* format,...);
    static int warn(const char *tag,const char* format,...);
    static int verbose(const char *tag,const char* format,...);
    static int debug(const char *tag,const char* format,...);
};
 */


#endif //KALASOFT_KSLOG_H
