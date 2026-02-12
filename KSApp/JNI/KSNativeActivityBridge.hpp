//
// Created by Shiva Shanker Pandiri on 2/10/26.
//

#ifndef KSGAMEACTIVITY_KSNATIVEACTIVITYBRIDGE_HPP
#define KSGAMEACTIVITY_KSNATIVEACTIVITYBRIDGE_HPP



#include<jni.h>
#include<functional>

class KSNativeActivityBridge {

public:

    static void init(JavaVM* vm, jobject activity);

    static void runOnJavaUIThread(std::function<void(JNIEnv*)> fn);

};


#endif //KSGAMEACTIVITY_KSNATIVEACTIVITYBRIDGE_HPP
