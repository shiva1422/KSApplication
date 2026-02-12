//
// Created by kalasoft on 12/23/21.
//

#ifndef KALASOFT_APPJAVACALLS_H
#define KALASOFT_APPJAVACALLS_H


#include <Graphics/Display.h>
#include <CMedia/KSImage.h>
#include <jni.h>
#include <game-text-input/gametextinput.h>

struct android_app;
class AppJavaCalls {


public:

    static void init(android_app *app);


    static android_app* getApp(){return app;}
    static bool getDisplayMetrics(DisplayMetrics &displayMetrics);

    static bool hideSystemUI();

    static void gotoPlayStore();

    static void openPrivacyPolicy();

    static void showBannerAd();

    static void dismissBannerAd();

    static bool showKeyboard();


    static KSImage* loadImageAsset(const char* path);

    static void onApplicationCreated(long appHandle);


    static KSImage *loadImageFile(const char *path);

    static void* createNativeAppInstance();

    static void goBack();

private:
    static JavaVM *vm;
    static android_app *app;
    static jobject globalActivity;
    static bool attachThread(bool &didAttach, JNIEnv **threadEnv);
    static void detachThread();

    static int attachCount;

};


#endif //KALASOFT_APPJAVACALLS_H
