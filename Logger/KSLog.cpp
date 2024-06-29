//
// Created by kalasoft on 9/30/21.
//

#include <cstdio>
#include "Logger/KSLog.h"
#include "android/log.h"

/*
//TODO why not androidlog instead of vprint
intKSLOGE(const char *tag, const char *format, ...)
{
    va_list args;
    int res;
    va_start(args,format);
    res = __android_log_vprint(ANDROID_LOG_ERROR, tag, format,args);
    va_end(args);
    return res;
}
intKSLOGI(const char *tag, const char *format, ...)
{
    va_list args;
    int res;
    va_start(args,format);
    res = __android_log_vprint(ANDROID_LOG_INFO, tag, format,args);
    va_end(args);
    return res;
}
intKSLOGW(const char *tag, const char *format, ...)
{
    va_list args;
    int res;
    va_start(args,format);
    res = __android_log_vprint(ANDROID_LOG_WARN, tag, format,args);
    va_end(args);
    return res;
}
int KSLOGV(const char *tag, const char *format, ...)
{
    va_list args;
    int res;
    va_start(args,format);
    res = __android_log_vprint(ANDROID_LOG_VERBOSE, tag, format,args);
    va_end(args);
    return res;
}

int KSLOGD(const char *tag, const char *format, ...)
{
    va_list args;
    int res;
    va_start(args,format);
    res = __android_log_vprint(ANDROID_LOG_DEBUG, tag, format,args);
    va_end(args);
    return res;
}

*/