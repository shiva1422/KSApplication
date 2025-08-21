//
// Created by Shiva Shanker Pandiri on 8/6/25.
//

#include "JNIImage.hpp"
#include <android/bitmap.h>
#include <Logger/KSLog.h>
#include <assert.h>

#define TAGJNI "JNIImage"


JNIImage::JNIImage(JNIEnv *env, jobject bitmap) {

   // this->env = penv;
   // this->bitmap  = env->NewGlobalRef(pbitmap);



    AndroidBitmapInfo bitmapInfo;
    if (AndroidBitmap_getInfo(env, reinterpret_cast<jobject>(bitmap), &bitmapInfo) < 0)
    {
        KSLOGE(TAGJNI,"coulnd not obtain bitmap info");
        return;
    }
    if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        KSLOGE(TAGJNI,"THE BITMAP FORMA TNOT NOT RGBA 8888,implement other formats");/////?improve to support others
        return;
    }

     width  = bitmapInfo.width;
     height = bitmapInfo.height;
     uint8_t* pixels = nullptr;
    // image.stride = bitmapInfo.stride;//TODO pixel format
    KSLOGD(TAGJNI,"load image- width-%d and height -%d",width,height);
    if (AndroidBitmap_lockPixels(env, bitmap, (void **) &pixels) != ANDROID_BITMAP_RESULT_SUCCESS)
    {
        KSLOGE(TAGJNI,"load image the bitmap could not be locked");
        assert(false);
        return;
    }


    data = (uint8_t*) malloc(width * height * 4);
    memcpy(data, pixels,width * height * 4);


    //setTexture of imageView or do anything with image and then unlock as this might be garbage collectore after return;
    AndroidBitmap_unlockPixels(env, bitmap);/////is unlock necessary ?



}

JNIImage::~JNIImage() {

   // if(bitmap != nullptr)
   // {
   //     env->DeleteGlobalRef(bitmap);//SHOULD BE SAME THREAD AS CREATED
  //  }
}

