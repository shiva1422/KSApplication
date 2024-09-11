//
// Created by kalasoft on 12/23/21.
//

#include <Logger/KSLog.h>
#include "AppJavaCalls.h"
#include <android/bitmap.h>
#include <assert.h>

#define TAGJNI "AppJavaCall"

jclass AppJavaCalls::cls;
JavaVM* AppJavaCalls::vm;
JNIEnv* AppJavaCalls::env;
android_app* AppJavaCalls::app;
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
        vm->AttachCurrentThread(&env, NULL);
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
    vm->DetachCurrentThread();

}

/*
 * check image valid after return
 */
KSImage* AppJavaCalls::loadImageAsset(const char *path)
{
    KSImage *image = nullptr;
    if(attachThreadAndFindClass())
    {
        jmethodID mid = env->GetMethodID(cls, "loadImageAsset", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
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

    detachThread();//can result in error, above else not covered

    return image;
}
