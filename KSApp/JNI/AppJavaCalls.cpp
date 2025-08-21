//
// Created by kalasoft on 12/23/21.
//

#include <Logger/KSLog.h>
#include "AppJavaCalls.h"
#include "JNIImage.hpp"
#include <android/bitmap.h>
#include <assert.h>

#define TAGJNI "AppJavaCall"

jclass AppJavaCalls::cls;
JavaVM* AppJavaCalls::vm;
JNIEnv* AppJavaCalls::env;
android_app* AppJavaCalls::app;

 int AppJavaCalls::attachCount = 0;

/*
 * TODO map function names and params instead of doing seperate for each function;
 * detachThead can be improved a bit
 * clear memory checks
 * */
bool AppJavaCalls::getDisplayMetrics(DisplayMetrics &displayMetrics)
{
    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "getDisplayParams", "()[F");
        if (mid == 0)
        {
           KSLOGE(TAGJNI,"error obtaining the method id get DisplayMetrics");
            detachThread();
            return false;
        }
        jfloatArray  displayParamsArray=(jfloatArray) env->CallObjectMethod(app->activity->clazz,mid);
        // jsize length=env->GetArrayLength(displayParamsArray);
        jfloat  *params=env->GetFloatArrayElements(displayParamsArray,0);
        displayMetrics.screenWidth=params[0];
        displayMetrics.screenHeight=params[1];
        displayMetrics.density=params[2];
        displayMetrics.densityDpi=params[3];
        displayMetrics.deviceStableDensity=params[4];
        displayMetrics.scaledDensity=params[5];
        displayMetrics.xdpi=params[6];
        displayMetrics.ydpi=params[7];
        env->ReleaseFloatArrayElements(displayParamsArray,params,0);
        detachThread();
        return true;
    }
    detachThread();
    return false;

}
bool AppJavaCalls::attachThreadAndFindClass()
{

    if(!app)
    {
        return false;
    }
        vm = app->activity->vm;
        if(vm->AttachCurrentThread(&env, NULL) == JNI_OK)
        {
            attachCount++;
        }
        if (!env)
        {
            KSLOGE(TAGJNI, "JNIEnv(null) %s and %d ", __func__ , __LINE__);
            detachThread();
            return false;
        }
        cls = (env)->GetObjectClass(app->activity->clazz);
        if(!cls)
        {
            KSLOGE(TAGJNI,"could not get java object class %s %d",__func__ ,__LINE__);
            detachThread();
            return false;
        }
        return true ;

}

void AppJavaCalls::detachThread()
{
    attachCount--;
    if(attachCount == 0)
    vm->DetachCurrentThread();//TODO thread specific,

}

/*
 * check image valid after return
 */
KSImage* AppJavaCalls::loadImageAsset(const char *path)
{
    JNIImage *image = nullptr;
    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "loadImageAsset", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get loadImageAsset");
            detachThread();
            return image;
        }

        KSLOGD(TAGJNI,"Import image %s",path);
        jstring filePathJava = env->NewStringUTF(path);//TODO vclear remember
        jobject bitmap = env->CallObjectMethod(app->activity->clazz, mid,filePathJava);

        if (bitmap != NULL)
        {
            env->NewLocalRef(bitmap);
            image = new JNIImage(env,bitmap);
            env->DeleteLocalRef(bitmap);
        }

    }

    detachThread();//can result in error, above else not covered

    return image;
}

void AppJavaCalls::gotoPlayStore() {

    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "gotoPlaystore", "()V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get gotoPlayStore");
            //detachThread();
            return ;
        }
        env->CallVoidMethod(app->activity->clazz,mid);
    }

    detachThread();
}

void AppJavaCalls::openPrivacyPolicy() {

    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "openPrivacyPolicy", "()V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get openPrivacy Policy");
            //detachThread();
            return ;
        }
        env->CallVoidMethod(app->activity->clazz,mid);
    }

    detachThread();
}

void AppJavaCalls::showBannerAd() {

    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "showBannerAd", "()V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get showBannerAd");
            //detachThread();
            return ;
        }
        env->CallVoidMethod(app->activity->clazz,mid);
    }

    detachThread();

}

void AppJavaCalls::dismissBannerAd() {

    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "dismissBannerAd", "()V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id dismissBannerAd");
            //detachThread();
            return ;
        }
        env->CallVoidMethod(app->activity->clazz,mid);
    }

    detachThread();

}

bool AppJavaCalls::toggleKeyboard() {

    JNIEnv *jni;
    app->activity->vm->AttachCurrentThread( &jni, NULL );

    jclass cls = jni->GetObjectClass(app->activity->clazz);
    jmethodID methodID = jni->GetMethodID(cls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;" );
    jstring service_name = jni->NewStringUTF("input_method");
    jobject input_service = jni->CallObjectMethod(app->activity->clazz, methodID, service_name);

    jclass input_service_cls = jni->GetObjectClass(input_service);
    methodID = jni->GetMethodID(input_service_cls, "toggleSoftInput", "(II)V");
    jni->CallVoidMethod(input_service, methodID, 0, 0);

    jni->DeleteLocalRef(service_name);

    app->activity->vm->DetachCurrentThread();

    return true;
}


void* AppJavaCalls::createNativeAppInstance() {

    void* NApp = nullptr;
    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "createMyNative", "()J");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get OnCreate");
            //detachThread();
            return NApp ;
        }

        jlong nApp =env->CallLongMethod(app->activity->clazz,mid);

        NApp = reinterpret_cast<void*>(nApp);
    }

    detachThread();

    return NApp;

}

void AppJavaCalls::onApplicationCreated(long appHandle) {

    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "onNativeAppCreated", "(J)V");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get OnCreate");
            //detachThread();
            return ;
        }
        env->CallVoidMethod(app->activity->clazz,mid,appHandle);
    }

    detachThread();

}

KSImage *AppJavaCalls::loadImageFile(const char *path) {

    KSImage *image = nullptr;
    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "loadImageFile", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        if (mid == 0)
        {
            KSLOGE(TAGJNI,"error obtaining the method id get loadImageAsset");
            detachThread();
            return image;
        }

        jstring filePathJava = env->NewStringUTF(path);//TODO vclear remember
        jobject bitmap = env->CallObjectMethod(app->activity->clazz, mid,filePathJava);
        if (bitmap != NULL)
        {
            KSLOGD(TAGJNI,"successfully obtained the bitmap");
            AndroidBitmapInfo bitmapInfo;
            if (AndroidBitmap_getInfo(env, reinterpret_cast<jobject>(bitmap), &bitmapInfo) < 0)
            {
                KSLOGE(TAGJNI,"coulnd not obtain bitmap info");
                return image;
            }
            if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
                KSLOGE(TAGJNI,"THE BITMAP FORMA TNOT NOT RGBA 8888,implement other formats");/////?improve to support others
                return image;
            }
            image = new KSImage();
            image->width  = bitmapInfo.width;
            image->height = bitmapInfo.height;
            // image.stride = bitmapInfo.stride;//TODO pixel format
            KSLOGD(TAGJNI,"load image- width-%d and height -%d",image->width,image->height);
            if (AndroidBitmap_lockPixels(env, bitmap, (void **) &image->data) < 0)
            {
                KSLOGE(TAGJNI,"load image the bitmap could not be locked");
                assert(false);
                delete image;
                image = nullptr;
                return image;
            }
            //setTexture of imageView or do anything with image and then unlock as this might be garbage collectore after return;
            AndroidBitmap_unlockPixels(env, bitmap);/////is unlock necessary ?
            env->DeleteLocalRef(bitmap);//TODO
        }
    }

    //detachThread();//can result in error, above else not covered//TODO

    return image;
}

