//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 11/25/24.
//

#ifndef KALASOFT_APPUTILS_H
#define KALASOFT_APPUTILS_H


class KSApplication;
class android_app;
class AppUtils {

public:

    static void gotoPlaystore();

    static void openPrivacyPolicy();

    static void showBanner();

    static void dismissBanner();

    static bool showKeyboard();

    static void onApplicationCreated(long appHandle);

    static KSApplication* createNativeAppInstance();

    static android_app* getAndroidApp();

    static void goBack();
};


#endif //KALASOFT_APPUTILS_H
