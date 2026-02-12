//
// Created by Shiva Shanker Pandiri on 2/10/26.
//

#include "KSNativeActivityBridge.hpp"
#include <jni.h>
#include <functional>


static JavaVM* kvm = nullptr;
static jobject kactivity = nullptr;
static jclass bridgeClass = nullptr;
static jmethodID runMethod = nullptr;

void KSNativeActivityBridge::init(JavaVM *vm, jobject activity) {

    kvm = vm;

    JNIEnv* env;
    if(vm->AttachCurrentThread(&env, nullptr) == JNI_OK)
    {
        kactivity = env->NewGlobalRef(activity);//TODO deleteRef.

        jclass cls = env->GetObjectClass(kactivity);
        bridgeClass = (jclass)env->NewGlobalRef(cls);//TODO
        runMethod = env->GetStaticMethodID(bridgeClass,"runNativeOnUIThread","(JLandroid/app/Activity;)V");
        vm->DetachCurrentThread();
    }

}

void KSNativeActivityBridge::runOnJavaUIThread(std::function<void(JNIEnv *)> fn) {

    JNIEnv *env;
    if(kvm->AttachCurrentThread(&env, nullptr) == JNI_OK)
    {
        auto heapFn = new std::function<void(JNIEnv*)>(fn);

        env->CallStaticVoidMethod(bridgeClass,runMethod,(jlong)heapFn,kactivity);

        kvm->DetachCurrentThread();

    }
}



extern "C"
JNIEXPORT void JNICALL
Java_com_kalasoft_ksgameactivity_KSGameActivity_nativeCall(JNIEnv *env, jclass clazz, jlong funct) {
    // TODO: implement nativeCall()
    auto fn = reinterpret_cast<std::function<void(JNIEnv*)>*>(funct);
    (*fn)(env);
    delete fn;
}