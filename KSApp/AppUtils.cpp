//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 11/25/24.
//

#include <KSApp/JNI/AppJavaCalls.h>
#include "AppUtils.h"

void AppUtils::gotoPlaystore() {

    AppJavaCalls::gotoPlayStore();
}

void AppUtils::openPrivacyPolicy() {

    AppJavaCalls::openPrivacyPolicy();

}

void AppUtils::showBanner() {

  AppJavaCalls::showBannerAd();
}

void AppUtils::dismissBanner() {

   AppJavaCalls::dismissBannerAd();
}

bool AppUtils::toggleKeyboardOpen() {
    AppJavaCalls::toggleKeyboard();
    return true;
}

void AppUtils::onApplicationCreated(long appHandle) {


    AppJavaCalls::onApplicationCreated(appHandle);
}


KSApplication *AppUtils::createNativeAppInstance() {

    return static_cast<KSApplication *>(AppJavaCalls::createNativeAppInstance());
}

android_app *AppUtils::getAndroidApp() {
    return AppJavaCalls::getApp();
}









