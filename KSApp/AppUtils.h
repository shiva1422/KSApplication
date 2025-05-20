//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 11/25/24.
//

#ifndef KALASOFT_APPUTILS_H
#define KALASOFT_APPUTILS_H


class AppUtils {

public:

    static void gotoPlaystore();

    static void openPrivacyPolicy();

    static void showBanner();

    static void dismissBanner();

    static bool toggleKeyboardOpen();

    static void onApplicationCreated(long appHandle);
};


#endif //KALASOFT_APPUTILS_H
