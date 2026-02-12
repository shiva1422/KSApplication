//
// Created by kalasoft on 12/23/21.
//

#include <Logger/KSLog.h>
#include "AppJavaCalls.h"
#include "JNIImage.hpp"
#include "KSNativeActivityBridge.hpp"
#include <android/bitmap.h>
#include <assert.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <game-text-input/gametextinput.h>
#include <KSApp/Events/KeyEvent.h>

#define TAGJNI "AppJavaCall"

JavaVM* AppJavaCalls::vm;
android_app* AppJavaCalls::app;

 int AppJavaCalls::attachCount = 0;

/*
 * TODO map function names and params instead of doing seperate for each function;
 * detachThead can be improved a bit
 * clear memory checks
 * reduce redundancies;
 * */
bool AppJavaCalls::getDisplayMetrics(DisplayMetrics &displayMetrics)
{

    bool didAttach;
    JNIEnv* threadEnv = nullptr;

    if(attachThread(didAttach, &threadEnv))
    {
        jclass cls = threadEnv->GetObjectClass(app->activity->javaGameActivity);

        jmethodID mid = threadEnv->GetMethodID(cls, "getDisplayParams", "()[F");
        if (mid == 0)
        {
           KSLOGE(TAGJNI,"error obtaining the method id get DisplayMetrics");
           if(didAttach)
            detachThread();
            return false;
        }
        jfloatArray  displayParamsArray=(jfloatArray) threadEnv->CallObjectMethod(app->activity->javaGameActivity,mid);
        // jsize length=env->GetArrayLength(displayParamsArray);
        jfloat  *params= threadEnv->GetFloatArrayElements(displayParamsArray,0);
        displayMetrics.screenWidth=params[0];
        displayMetrics.screenHeight=params[1];
        displayMetrics.density=params[2];
        displayMetrics.densityDpi=params[3];
        displayMetrics.deviceStableDensity=params[4];
        displayMetrics.scaledDensity=params[5];
        displayMetrics.xdpi=params[6];
        displayMetrics.ydpi=params[7];
        threadEnv->ReleaseFloatArrayElements(displayParamsArray,params,0);

        if(didAttach)
        detachThread();
        return true;
    }

    return false;

}
bool AppJavaCalls::attachThread(bool &didAttach, JNIEnv **threadEnv)
{

    didAttach = false;

    if(!app)
    {
        assert(false);
        return false;
    }

    vm = app->activity->vm;

    if(vm->GetEnv((void **)threadEnv,JNI_VERSION_1_6) != JNI_OK)
    {
        if (vm->AttachCurrentThread(threadEnv, nullptr) != JNI_OK)
        {
            KSLOGE(TAGJNI, "AttachCurrentThread failed");
            *threadEnv = nullptr;
            return false;
        }
        didAttach = true;
    }


    if (!(*threadEnv))
    {
        KSLOGE(TAGJNI, "JNIEnv(null) %s and %d ", __func__ , __LINE__);

        didAttach = false;
        return false;

    }

    return true ;

}

void AppJavaCalls::detachThread()
{
    vm->DetachCurrentThread();//TODO thread specific,
}

/*
 * check image valid after return
 */
KSImage* AppJavaCalls::loadImageAsset(const char *path)
{
    JNIImage *image = nullptr;

    bool didAttach;
    JNIEnv* threadEnv = nullptr;
    if(attachThread(didAttach, &threadEnv))
    {

        jclass cls = threadEnv->GetObjectClass(app->activity->javaGameActivity);

        jmethodID mid = threadEnv->GetMethodID(cls, "loadImageAsset", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get loadImageAsset");
            if(didAttach)
            detachThread();
            didAttach = false;
            return image;
        }

        KSLOGD(TAGJNI,"Import image %s",path);
        jstring filePathJava = threadEnv->NewStringUTF(path);//TODO vclear remember
        jobject bitmap = threadEnv->CallObjectMethod(app->activity->javaGameActivity, mid,filePathJava);

        if (bitmap != NULL)
        {
            threadEnv->NewLocalRef(bitmap);
            image = new JNIImage(threadEnv,bitmap);
            threadEnv->DeleteLocalRef(bitmap);
        }

    }else
    {
        assert(false);
    }

    if(didAttach)
    detachThread();//can result in error, above else not covered

    return image;
}

KSImage *AppJavaCalls::loadImageFile(const char *path) {

    KSImage *image = nullptr;

    bool didAttach;
    JNIEnv* threadEnv = nullptr;
    if(attachThread(didAttach, &threadEnv))
    {
        jclass cls = threadEnv->GetObjectClass(app->activity->javaGameActivity);

        jmethodID mid = threadEnv->GetMethodID(cls, "loadImageFile", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get loadImageAsset");
            if(didAttach)
            detachThread();
            didAttach = false;
            return image;
        }

        jstring filePathJava = threadEnv->NewStringUTF(path);//TODO vclear remember
        jobject bitmap = threadEnv->CallObjectMethod(app->activity->javaGameActivity, mid,filePathJava);
        if (bitmap != NULL)
        {

            threadEnv->NewLocalRef(bitmap);
            image = new JNIImage(threadEnv,bitmap);
            threadEnv->DeleteLocalRef(bitmap);

        }
    }else
        {
            assert(false);
        }

    if(didAttach)
    detachThread();//can result in error, above else not covered//TODO

    return image;
}


void AppJavaCalls::gotoPlayStore() {

    bool didAttach;
    JNIEnv* threadEnv = nullptr;
    if(attachThread(didAttach, &threadEnv))
    {
        jclass cls = threadEnv->GetObjectClass(app->activity->javaGameActivity);

        jmethodID mid = threadEnv->GetMethodID(cls, "gotoPlaystore", "()V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get gotoPlayStore");
            if(didAttach)
            detachThread();
            return ;
        }
        threadEnv->CallVoidMethod(app->activity->javaGameActivity,mid);
    }

    if(didAttach)
    detachThread();
}

void AppJavaCalls::openPrivacyPolicy() {

    bool didAttach;
    JNIEnv* threadEnv = nullptr;
    if(attachThread(didAttach, &threadEnv))
    {
        jclass cls = threadEnv->GetObjectClass(app->activity->javaGameActivity);


        jmethodID mid = threadEnv->GetMethodID(cls, "openPrivacyPolicy", "()V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get openPrivacy Policy");
            if(didAttach)
            detachThread();
            return ;
        }
        threadEnv->CallVoidMethod(app->activity->javaGameActivity,mid);
    }

    if(didAttach)
    detachThread();
}

void AppJavaCalls::showBannerAd() {

    bool didAttach;
    JNIEnv* threadEnv = nullptr;
    if(attachThread(didAttach, &threadEnv))
    {

        jclass cls = threadEnv->GetObjectClass(app->activity->javaGameActivity);

        jmethodID mid = threadEnv->GetMethodID(cls, "showBannerAd", "()V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get showBannerAd");
            if(didAttach)
            detachThread();
            return ;
        }
        threadEnv->CallVoidMethod(app->activity->javaGameActivity,mid);
    }

    detachThread();

}

void AppJavaCalls::dismissBannerAd() {

    bool didAttach;
    JNIEnv* threadEnv = nullptr;
    if(attachThread(didAttach, &threadEnv))
    {
        jclass cls = threadEnv->GetObjectClass(app->activity->javaGameActivity);

        jmethodID mid = threadEnv->GetMethodID(cls, "dismissBannerAd", "()V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id dismissBannerAd");
            if(didAttach)
            detachThread();
            return ;
        }
        threadEnv->CallVoidMethod(app->activity->javaGameActivity,mid);
    }

    if(didAttach)
    detachThread();

}

bool AppJavaCalls::showKeyboard() {

   KSNativeActivityBridge::runOnJavaUIThread([&](JNIEnv* env){
       if(ks::KeyboardController::gameTextInput == nullptr)
       {
           ks::KeyboardController::gameTextInput = GameTextInput_init(env,1024);//TODO
       }

       GameTextInput_setEventCallback(ks::KeyboardController::gameTextInput,ks::KeyboardController::onTextInput, nullptr);
       GameTextInput_showIme(ks::KeyboardController::gameTextInput,0);


   });
    return true;
}


void* AppJavaCalls::createNativeAppInstance() {

    void* NApp = nullptr;
    bool didAttach;
    JNIEnv* threadEnv = nullptr;
    if(attachThread(didAttach, &threadEnv))
    {
        jclass cls = threadEnv->GetObjectClass(app->activity->javaGameActivity);

        jmethodID mid = threadEnv->GetMethodID(cls, "createMyNative", "()J");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get OnCreate");
            if(didAttach)
            detachThread();
            return NApp ;
        }

        jlong nApp =threadEnv->CallLongMethod(app->activity->javaGameActivity,mid);

        NApp = reinterpret_cast<void*>(nApp);
    }

    if(didAttach)
    detachThread();

    return NApp;

}

void AppJavaCalls::onApplicationCreated(long appHandle) {

    bool didAttach;
    JNIEnv* threadEnv = nullptr;
    if(attachThread(didAttach, &threadEnv))
    {
        jclass cls = threadEnv->GetObjectClass(app->activity->javaGameActivity);

        jmethodID mid = threadEnv->GetMethodID(cls, "onNativeAppCreated", "(J)V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get OnCreate");
            if(didAttach)
            detachThread();
            return ;
        }
        threadEnv->CallVoidMethod(app->activity->javaGameActivity,mid,appHandle);
    }

    if(didAttach)
    detachThread();

}

void AppJavaCalls::goBack() {

    bool didAttach;
    JNIEnv* threadEnv = nullptr;
    if(attachThread(didAttach, &threadEnv))
    {
        jclass cls = threadEnv->GetObjectClass(app->activity->javaGameActivity);

        jmethodID mid = threadEnv->GetMethodID(cls, "goBack", "()V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get OnCreate");
            if(didAttach)
            detachThread();
            return ;
        }else
        {
            KSLOGD(TAGJNI,"Goint to Back Activity :");
            threadEnv->CallVoidMethod(getApp()->activity->javaGameActivity,mid);
        }
    }

    if(didAttach)
    detachThread();
}



void AppJavaCalls::init(android_app *app) {

        AppJavaCalls::app=app;

        bool didattach;
        JNIEnv *env;
        if(attachThread(didattach,&env))
        {
            globalActivity = env->NewGlobalRef(app->activity->javaGameActivity);;//TODO delete;

        }

        if(didattach)detachThread();


}


jobject AppJavaCalls::globalActivity = nullptr;