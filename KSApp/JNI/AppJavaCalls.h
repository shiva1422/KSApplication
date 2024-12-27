//
// Created by kalasoft on 12/23/21.
//

#ifndef KALASOFT_APPJAVACALLS_H
#define KALASOFT_APPJAVACALLS_H


#include <android_native_app_glue.h>
#include <Graphics/Display.h>
#include <CMedia/KSImage.h>

class AppJavaCalls {


public:

    static void init(android_app *app)
    {
        AppJavaCalls::app=app;
    }

    static bool getDisplayMetrics(DisplayMetrics &displayMetrics);

    static bool hideSystemUI();

    static void gotoPlayStore();

    static void openPrivacyPolicy();

    static void showBannerAd();

    static void dismissBannerAd();


    static KSImage* loadImageAsset(const char* path);

private:
    static jclass cls;
    static JavaVM *vm;
    static JNIEnv *env;
    static android_app *app;
    static bool attachThreadAndFindClass();
    static void detachThread();
};


#endif //KALASOFT_APPJAVACALLS_H
