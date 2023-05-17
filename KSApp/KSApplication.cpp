//
// Created by kalasoft on 10/5/21.
//

#include <cassert>
#include "JNI/AppJavaCalls.h"
#include <Graphics/KSUI/GL/GLView.h>
#include <unistd.h>
#include "KSApplication.h"
#include <Logger/KSLog.h>
#include "Events/AndroidEvents.h"
#include "Events/CustomEvents.h"
#include "KSApp/IO/KSAssetReader.h"
#include "android/log.h"

#define APPTAG  "KSApplication"
#define AppLogV(x) KSLog::verbose(APPTAG,x)

//Destructor and destroy app.
KSApplication::KSApplication(android_app *papp,std::string appName)
{

    this->appName = appName;
    app = papp;
    AndroidEvents::setMotionEventInterceptor(this);
    app->onAppCmd = AndroidEvents::onAppCmd;
    app->onInputEvent = AndroidEvents::onInputEvent;
    app->userData = this;
    mAssetManager = this;

    AppJavaCalls::init(app);
    assert(updateDisplayMetrics());
    assert(renderer.init());

    customEvents = new CustomEvents(this);

    pthread_setname_np(pthread_self(),appName.c_str());
    //TODO after Created looop();
}

KSApplication::~KSApplication()
{

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

    }while(true);//(app->destroyRequested==0);
    //dont exit until destroyed requested as only this thread exits not the Activity(apps main thread).  if want to exit call ANativeActivity_finish(or APP_CMD_DESTROY)

}

void KSApplication::onDraw()
{
    //TODO check accurat bWindowInits
    if(bWindowInit)renderer.onRender();
    else
        KSLog::warn(APPTAG,"onDraw window in not initialized");
}

void KSApplication::onScreenRotation()
{

}

//CallBacks
void KSApplication::onWindowInit()
{
    KSLog::verbose(APPTAG,"WindowInit");
    //JavaCall::hideSystemUI();//TODO some thing check screen dimension for differnt cases like having navigation bars?
   // ANativeWindow_setBuffersGeometry(app->window,displayMetrics.screenWidth,displayMetrics.screenHeight,ANativeWindow_getFormat(app->window));
    window.setWindow(app->window);
    renderer.setSurfaceWindow(window);
    bWindowInit=true;
   //TODO a lot..............?
    if (bAppFirstOpen)
    {
        /* assert(Graphics::init_display(this) == STATUS_OK);
         bAppFirstOpen = false;
         bWindowInit = true;
         bGraphicsInit = true;
         return;*/
        // bGraphicsInit=vulkanContext.initialize(app);
        if(!bGraphicsInit)
        {
            //bGraphicsInit = glContext.init();
          //  GLuint shaderId = Shader::createShaderProgram("shaders/ui/vertexShader.glsl","shaders/ui/fragmentShader.glsl");
           // glUseProgram(shaderId);
        }

        bWindowInit = true;
    }
    else
    {
      //  glContext.onAppReopen();
    }
}
void KSApplication::onWindowResized()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT :WindowResized");
}
void KSApplication::onWindowRedrawNeeded()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT :Redraw Needed");
}
void KSApplication::onWindowTermination()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT : Terminating");
}
void KSApplication::onContentRectChanged()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT :Content Rect Changed");
}
void KSApplication::onCreate()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT :OnCreate");
}
void KSApplication::onStart()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT :STart");
}
void KSApplication::onResume()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT :Resume");
}
void KSApplication::onPause()
{
    bWindowInit= false;
    KSLog::debug(this->appName.c_str(),"KS EVENT :Pause");
}
void KSApplication::onStop()
{
    bWindowInit= false;
    KSLog::debug(this->appName.c_str(),"KS EVENT :Stop");
}
void KSApplication::onDestroy()
{
    bWindowInit= false;
    KSLog::debug(this->appName.c_str(),"KS EVENT :Destroy");
}
void KSApplication::onFocusGained()
{
     KSLog::debug(this->appName.c_str(),"KS EVENT :Focus Gained");
}
void KSApplication::onFocusLost()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT :Focus Lost");
}
void KSApplication::onConfigChanged()
{
    //TODO get same config always
    KSLog::debug(this->appName.c_str(),"KS EVENT :Config changed");
    if(AConfiguration_getOrientation(app->config) == ACONFIGURATION_ORIENTATION_PORT)
        KSLog::debug(this->appName.c_str(),"KS EVENT :Portrait");
    else
        KSLog::debug(this->appName.c_str(),"KS EVENT :Landscape");
    //AppLogI("%d and orientation %d",AConfiguration_getScreenHeightDp(app->config),AConfiguration_getOrientation(app->config));
    // glViewport(0,0,displayMetrics.screenHeight,displayMetrics.screenWidth);
}
void KSApplication::onSaveState()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT :Save State");
}
void KSApplication::onLowMemory()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT :Low memory");
}
void KSApplication::onInputChange()
{
    KSLog::debug(this->appName.c_str(),"KS EVENT :Input Change");
}

bool KSApplication ::updateDisplayMetrics()
{//TODO statick
    bool res = AppJavaCalls::getDisplayMetrics(displayMetrics);
    Renderer::setDisplayMetrics(displayMetrics);
    displayMetrics.print();
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


        KSLog::info("KSApp","getAssetFD %d in %d out %lld",fd, lseek(fd,0,SEEK_SET),end);
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
        KSLog::error("OpenAsset","Failed %s",assetPath);

    return reader;
}
//TODO UI Below
void KSApplication::setContentView(const View *content)
{

    renderer.setContent((void *)content);//TODO CONST NOOO?
}

View *KSApplication::getContentView() const
{

    //Make sure View* is retruned or
    return static_cast<View *>(renderer.getContent());
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



//Static Memebers

AssetManager* AssetManager::mAssetManager = nullptr;