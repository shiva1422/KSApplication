//
// Created by kalasoft on 10/5/21.
//

#include <cassert>
#include "JNI/AppJavaCalls.h"
#include <Graphics/KSUI/GL/GLView.h>
#include <unistd.h>
#include "KSApplication.h"
#include <Logger/KSLog.h>
#include <KSIO/AssetManager.h>
#include <CMedia/KSImage.h>
#include <KSUI/Renderer/VulkanUIRenderer.h>
#include "Events/AndroidEvents.h"
#include "Events/CustomEvents.h"
#include "KSApp/IO/KSAssetReader.h"
#include "android/log.h"

#define APPTAG  "KSApplication"
#define AppLogV(x) KSLOGV(APPTAG,x)

//Destructor and destroy app.
KSApplication::KSApplication(android_app *papp,std::string appName)
{

    this->appName = appName;
    app = papp;
    AndroidEvents::setMotionEventInterceptor(this);
    AndroidEvents::setKeyEventInterceptor(this);
    app->onAppCmd = AndroidEvents::onAppCmd;
    app->onInputEvent = AndroidEvents::onInputEvent;
    app->userData = this;
    mAssetManager = this;

    AppJavaCalls::init(app);
    if(bUseGL)
    renderer = new GLUIRenderer();
    else
    {
        KSLOGE(APPTAG,"unimplemented vulkan");
        //renderer = new VulkanUIRenderer();
    }
    assert(updateDisplayMetrics());
    assert(renderer->init());

    customEvents = new CustomEvents(this);

    pthread_setname_np(pthread_self(),appName.c_str());
    //TODO after Created looop();
    //Shaders Init
    //
}

KSApplication::~KSApplication()
{
    //Clear Shaders
    //Clear Graphics Context;
}

void KSApplication::run()
{
    this->onCreate();
    int eventId,events,fdesc;
    android_poll_source* source;
    do
    {
        runTasks();//here or after event/berfore render ? TODO
        if((eventId=ALooper_pollAll(0,&fdesc,&events,(void **) &source))>=0)
        {
            if(source != NULL)
            {
                source->process(app,source);
            }

        }

        onDraw();

    }while(!bAppDestroyed);//(app->destroyRequested==0);
    //dont exit until destroyed requested as only this thread exits not the Activity(apps main thread).  if want to exit call ANativeActivity_finish(or APP_CMD_DESTROY)

}

void KSApplication::onDraw()
{
    //TODO check accurat bWindowInits
    if(bWindowInit)renderer->onRender();
    else
    {
      //  KSLOGW(APPTAG,"onDraw window in not initialized");
    }
}

void KSApplication::onScreenRotation()
{
    KSLOGD(APPTAG,"Screen Rotation");
}

//CallBacks
void KSApplication::onWindowInit()
{
    KSLOGD(APPTAG,"WindowInit");
    //JavaCall::hideSystemUI();//TODO some thing check screen dimension for differnt cases like having navigation bars?
   // ANativeWindow_setBuffersGeometry(app->window,displayMetrics.screenWidth,displayMetrics.screenHeight,ANativeWindow_getFormat(app->window));
    renderer->setWindow(&window);
    bWindowInit=true;
}
void KSApplication::onWindowResized()
{
    KSLOGD(this->appName.c_str(),"KS EVENT :WindowResized");
}
void KSApplication::onWindowRedrawNeeded()
{
    KSLOGD(this->appName.c_str(),"KS EVENT :Redraw Needed");
}
void KSApplication::onWindowTermination()
{
    KSLOGD(this->appName.c_str(),"KS EVENT : Window Terminating");
}
void KSApplication::onContentRectChanged()
{
    KSLOGD(this->appName.c_str(),"KS EVENT :Content Rect Changed");
}
void KSApplication::onCreate()
{
    KSLOGD(this->appName.c_str(),"KS EVENT :OnCreate");
}
void KSApplication::onStart()
{
    KSLOGD(this->appName.c_str(),"KS EVENT :STart");
}
void KSApplication::onResume()
{
    KSLOGD(this->appName.c_str(),"KS EVENT :Resume");
}
void KSApplication::onPause()
{
    bWindowInit= false;
    KSLOGD(this->appName.c_str(),"KS EVENT :Pause");
}
void KSApplication::onStop()
{
    bWindowInit= false;
    KSLOGD(this->appName.c_str(),"KS EVENT :Stop");
}
void KSApplication::onDestroy()
{
    bWindowInit= false;
    bAppDestroyed = true;
    KSLOGD(this->appName.c_str(),"KS EVENT :Destroy");
}
void KSApplication::onFocusGained()
{
     KSLOGD(this->appName.c_str(),"KS EVENT :Focus Gained");
}
void KSApplication::onFocusLost()
{
    KSLOGD(this->appName.c_str(),"KS EVENT :Focus Lost");
}
void KSApplication::onConfigChanged()
{
    //TODO get same config always
    KSLOGD(this->appName.c_str(),"KS EVENT :Config changed");
    if(AConfiguration_getOrientation(app->config) == ACONFIGURATION_ORIENTATION_PORT)
        KSLOGD(this->appName.c_str(),"KS EVENT :Portrait");
    else
        KSLOGD(this->appName.c_str(),"KS EVENT :Landscape");
    //AppLogI("%d and orientation %d",AConfiguration_getScreenHeightDp(app->config),AConfiguration_getOrientation(app->config));
    // glViewport(0,0,displayMetrics.screenHeight,displayMetrics.screenWidth);
}
void KSApplication::onSaveState()
{
    KSLOGD(this->appName.c_str(),"KS EVENT :Save State");
}
void KSApplication::onLowMemory()
{
    KSLOGD(this->appName.c_str(),"KS EVENT :Low memory");
}
void KSApplication::onInputChange()
{
    KSLOGD(this->appName.c_str(),"KS EVENT :Input Change");
}

bool KSApplication ::updateDisplayMetrics()
{//TODO statick
    bool res = AppJavaCalls::getDisplayMetrics(displayMetrics);
    Renderer::setDisplayMetrics(displayMetrics);
    return res;
}

void KSApplication::forceUpdateDisplayMetrics(DisplayMetrics &displayMetrics)
{
    Renderer::setDisplayMetrics(displayMetrics);
}



AAssetManager* KSApplication::getAssetManager()
{
    //TODO AssetManager
    if(app)
    {
       return app->activity->assetManager;
    }
    return nullptr;
}
int KSApplication::getAssetFD(const char *assetLoc)
{
    int fd = -1;
    //TODO AssetManager
    AAsset *asset = AAssetManager_open(getAssetManager(),assetLoc,AASSET_MODE_RANDOM);
    //AAsset_close(asset);//CLose after done
    if(asset)
    {
        off_t start=0,end;
        fd = AAsset_openFileDescriptor(asset,&start,&end);
      //  AAsset_seek(asset,0,SEEK_SET);


       KSLOGI("KSApp","getAssetFD %d in %d out %lld",fd, lseek(fd,0,SEEK_SET),end);
    }
  //  AAsset_close(asset);
    return fd;
}

IKSStream *KSApplication::_openAsset(const char *assetPath)
{

    AAsset *asset = AAssetManager_open(getAssetManager(),assetPath,AASSET_MODE_RANDOM);
    IKSStream *reader = nullptr;
    if(asset)
    {
        reader = new KSAssetReader(asset);
    }
    else
       KSLOGE("OpenAsset","Failed %s",assetPath);

    return reader;
}
//TODO UI Below
void KSApplication::setContentView(const View *content)
{

    renderer->setContent((void *)content);//TODO CONST NOOO?
}

View *KSApplication::getContentView() const
{

    //Make sure View* is retruned or
    return static_cast<View *>(renderer->getContent());
}

AAsset *KSApplication::getAsset(const char *assetPath)
{

    return AAssetManager_open(getAssetManager(),assetPath,AASSET_MODE_RANDOM);
}

using namespace ks;
/* Entry point of touch/mouse event into application*/
bool KSApplication::onInterceptMotionEvent(const ks::MotionEvent  &me)
{
    //Implement is subclass for now later move to right place
    return false;
}

bool KSApplication::onInterceptKeyEvent(const KeyEvent &ke) {
    return false;
}



//Static Memebers

KSImage* KSApplication:: _loadImageAsset(const char *path)
{
    return AppJavaCalls::loadImageAsset(path);

}

AssetManager* AssetManager::mAssetManager = nullptr;

