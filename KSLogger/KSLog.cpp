//
// Created by kalasoft on 9/30/21.
//

#include <cstdio>
#include "KSLog.h"
#include "android/log.h"

//TODO why not androidlog instead of vprint
int KSLog::error(const char *tag, const char *format, ...)
{
    va_list args;
    int res;
    va_start(args,format);
    res = __android_log_vprint(ANDROID_LOG_ERROR, tag, format,args);
    va_end(args);
    return res;
}
int KSLog::info(const char *tag, const char *format, ...)
{
    va_list args;
    int res;
    va_start(args,format);
    res = __android_log_vprint(ANDROID_LOG_INFO, tag, format,args);
    va_end(args);
    return res;
}
int KSLog::warn(const char *tag, const char *format, ...)
{
    va_list args;
    int res;
    va_start(args,format);
    res = __android_log_vprint(ANDROID_LOG_WARN, tag, format,args);
    va_end(args);
    return res;
}
int KSLog::verbose(const char *tag, const char *format, ...)
{
    va_list args;
    int res;
    va_start(args,format);
    res = __android_log_vprint(ANDROID_LOG_VERBOSE, tag, format,args);
    va_end(args);
    return res;
}

int KSLog::debug(const char *tag, const char *format, ...)
{
    va_list args;
    int res;
    va_start(args,format);
    res = __android_log_vprint(ANDROID_LOG_DEBUG, tag, format,args);
    va_end(args);
    return res;
}