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







