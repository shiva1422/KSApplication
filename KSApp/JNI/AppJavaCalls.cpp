//
// Created by kalasoft on 12/23/21.
//

#include <Logger/KSLog.h>
#include "AppJavaCalls.h"

#define TAGJNI "AppJavaCall"

jclass AppJavaCalls::cls;
JavaVM* AppJavaCalls::vm;
JNIEnv* AppJavaCalls::env;
android_app* AppJavaCalls::app;
//TODO map function names and params instead of doing seperate for each function;
bool AppJavaCalls::getDisplayMetrics(DisplayMetrics &displayMetrics)
{
    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "getDisplayParams", "()[F");
        if (mid == 0)
        {
           KSLOGE(TAGJNI,"error obtaining the method id get DisplayMetrics");
            detachThread();
            return false;
        }
        jfloatArray  displayParamsArray=(jfloatArray) env->CallObjectMethod(app->activity->clazz,mid);
        // jsize length=env->GetArrayLength(displayParamsArray);
        jfloat  *params=env->GetFloatArrayElements(displayParamsArray,0);
        displayMetrics.screenWidth=params[0];
        displayMetrics.screenHeight=params[1];
        displayMetrics.density=params[2];
        displayMetrics.densityDpi=params[3];
        displayMetrics.deviceStableDensity=params[4];
        displayMetrics.scaledDensity=params[5];
        displayMetrics.xdpi=params[6];
        displayMetrics.ydpi=params[7];
        env->ReleaseFloatArrayElements(displayParamsArray,params,0);
        detachThread();
        return true;
    }
    detachThread();
    return false;

}
bool AppJavaCalls::attachThreadAndFindClass()
{

    if(!app)
    {
        return false;
    }
        vm = app->activity->vm;
        vm->AttachCurrentThread(&env, NULL);
        if (!env)
        {
            KSLOGE(TAGJNI, "JNIEnv(null) %s and %d ", __func__ , __LINE__);
            detachThread();
            return false;
        }
        cls = (env)->GetObjectClass(app->activity->clazz);
        if(!cls)
        {
            KSLOGE(TAGJNI,"could not get java object class %s %d",__func__ ,__LINE__);
            detachThread();
            return false;
        }
        return true ;

}

void AppJavaCalls::detachThread()
{
    vm->DetachCurrentThread();

}