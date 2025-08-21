//
// Created by Shiva Shanker Pandiri on 8/6/25.
//

#ifndef KALASOFT_JNIIMAGE_HPP
#define KALASOFT_JNIIMAGE_HPP


#include <CMedia/KSImage.h>
#include<jni.h>

class JNIImage : public KSImage{


public:

     JNIImage(JNIEnv *penv,jobject bitmap);

    ~JNIImage() override;

private:

   // JNIEnv * env = nullptr;
   // jobject bitmap = nullptr;
};




#endif //KALASOFT_JNIIMAGE_HPP
