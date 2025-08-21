//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 11/25/24.
//

#ifndef KALASOFT_APPUTILS_H
#define KALASOFT_APPUTILS_H


class KSApplication;
class AppUtils {

public:

    static void gotoPlaystore();

    static void openPrivacyPolicy();

    static void showBanner();

    static void dismissBanner();

    static bool toggleKeyboardOpen();

    static void onApplicationCreated(long appHandle);

    static KSApplication* createNativeAppInstance();

    static android_app* getAndroidApp();
};


#endif //KALASOFT_APPUTILS_H
