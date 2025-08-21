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
#include "Events/AppEvents.h"
#include "KSApp/IO/KSAssetReader.h"
#include "android/log.h"
#include "AppUtils.h"
#include <Shader.h>
#include <KSIO/FileManager.h>
#include <vector>

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
    fileManager = this;


    updateDisplayMetrics();

    if(bUseGL)
    {
        renderer = new GLRenderer();
        renderer->init();
        ((GLRenderer *)renderer)->enableBlending();
    }

    else
    {
        KSLOGE(APPTAG,"unimplemented vulkan");
        //renderer = new VulkanUIRenderer();
    }
    KSLOGV(appName.c_str(),"prepare Shaders %d", Shader::prepareShaders(this));//TODO move to application/Graphics aptly


    appEvents = new AppEvents(this);

    pthread_setname_np(pthread_self(),appName.c_str());
    //TODO after Created looop();
    //Shaders Init
    //
}

KSApplication::~KSApplication()
{
    //Clear Shaders
    //Clear Graphics Context;

    Shader::clearShaders();
    delete renderer;
    delete appEvents;

}

void KSApplication::run()
{
    this->onCreate();
    int eventId,events,fdesc;
    android_poll_source* source;


    std::chrono::duration<double> delta;
    previousFrameClock = std::chrono::system_clock::now();
    bool bPoll = false;
    do
    {
        if((eventId = ALooper_pollOnce(0,&fdesc,&events,(void **) &source)) >= 0)
        {
            if(source != NULL)
            {
                source->process(app,source);
                bPoll = true;
            }
        }

        runTasks();

        frameClock = std::chrono::system_clock::now();
        delta = frameClock - previousFrameClock;
        if(delta.count() >= 0.032 || bPoll)//30fps
        {
            onDraw();//TODO frame range management;
            previousFrameClock = std::chrono::system_clock::now();
            bPoll = false;
        }


    }while(!bAppDestroyed);//(app->destroyRequested==0);
    //dont exit until destroyed requested as only this thread exits not the Activity(apps main thread).  if want to exit call ANativeActivity_finish(or APP_CMD_DESTROY)

    KSLOGD(appName.c_str(),"Exiting");

}

void KSApplication::onDraw()
{
    //TODO check accurat bWindowInits
    if(bWindowInit)renderer->onRender();
    else
    {
        KSLOGW(APPTAG,"onDraw window in not initialized");
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
    renderer->setWindow(&window);
    bWindowInit=true;

    int rW = renderer->getWidth();
    int rH = renderer->getHeight();
    if((rW != displayMetrics.screenWidth || rH != displayMetrics.screenHeight) && rW > 0 && rH > 0)
    {
        KSLOGW(this->appName.c_str(),"DisplayMetrics and RenderWIndow (ANativeWindow ) size mismatch DM(%d,%d) window(%d,%d)",displayMetrics.screenWidth,displayMetrics.screenHeight,rW,rH);

        displayMetrics.screenWidth = rW;
        displayMetrics.screenHeight = rH;

        forceUpdateDisplayMetrics(displayMetrics);

        resizeUI();

        KSLOGW("KS Event","resize UI");
    }

    KSLOGD("onWindowInit","RenderWindow set");

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
    //renderer->setWindow(&window);
    bWindowInit = false;
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
    bWindowInit = false;
    KSLOGD(this->appName.c_str(),"KS EVENT :Stop");
}
void KSApplication::onDestroy()
{
    bWindowInit= false;//TODO order
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

    KSLOGD(appName.c_str(),"Opening Asset %s",assetPath);
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
    //Move logic here to a touch Manager;

    View *content = getContentView();
    if(!content)return false;//Right? if contentView Changes possible? no

    //if(!content->isPointInside(touchX,touchY)) return false;//TODO now only handling oneView check handling in ViewGroups

    int32_t pointerIndex = me.getPointerIndex();
    int32_t pointerId = me.getPointerId(pointerIndex);
    float touchX = me.getX(pointerIndex);
    float touchY = me.getY(pointerIndex);


    static std::unordered_set<TouchID> touchesActive;//

    switch(me.getAction())
    {
        case EMotionEventAction::DOWN:
        {
            KSLOGD("KSEVENT", " Action down Id - %d, index %d",pointerId,pointerIndex);

            if(content->isPointInside(touchX,touchY))
            {
                //TODO later avoid or do as required
                return View::dispatchTouchDown(content,touchX,touchY,pointerId,true);
            }
        }break;

        case EMotionEventAction::POINTER_DOWN:
        {
            KSLOGD("KSEVENT", " PointerDown Id - %d, index %d",pointerId,pointerIndex);

            if(content->isPointInside(touchX,touchY))
            {
                //TODO later avoid or do as required
                return View::dispatchTouchDown(content,touchX,touchY,pointerId,false);
            }
        }break;

        case EMotionEventAction::MOVE:
        {
            KSLOGD("KSEVENT", " Action move Id - %d, index %d",pointerId,pointerIndex);//The id index would be first one when multiple touches active

            int32_t pointerCount = me.getPointerCount();
            //processing for all pointers call only if historyValue and current value differ for particular index
            //TODO maybe need to check touch event history cause when swipes speed across screen some ponts may be missiong
            //therfore some enents may not be processed or events may be repeated check;
            KSLOGD("Action move:","count %d",pointerCount);
            for(int i = 0;i < pointerCount; ++i)
            {
                touchX = me.getX(i);
                touchY = me.getY(i);
                pointerId = me.getPointerId(i);//TODO check historical value changed and dispatch,move else moving one finger will trigger motionevent on all view with fingers that haven't moved;

                /*
               //if(tThe view/touch listener handling this pointerId as touchdown/pointer down or hoveenter maybe.
               //      if touch inside that view forward motion event to that view
               //      else forward hoverexit to the view and hover enter to the view which has touch inside
                *else getView as this location forward hover/enter/move?
                */

                if(View::isHandlingTouch(content, pointerId))
                {
                    if(content->isPointInside(touchX,touchY))
                    {
                        View::dispatchMove(content,touchX,touchY,pointerId);//todo result?
                    }
                    else
                    {
                        View::dispatchHoverExit(content,touchX,touchY,pointerId);//also hover enter to view at location
                        //TODO hover exit of content and hover enter to whatever View is at this location since content view is the only one, no view for hover enter.
                    }

                }
                else
                {
                    //TODO
                    KSLOGE("KSEVENT"," action moveunhandled Id - %d, index %d",pointerId,pointerIndex);
                }

            }

            //TODO return ?
        }break;
        case EMotionEventAction::POINTER_UP:
        {
            KSLOGD("KSEVENT", " Action pointer up Id - %d, index %d",pointerId,pointerIndex);
            //Consider hover enter exit TODO
            if(content->isPointInside(touchX,touchY))
            {
                //TODO later avoid or do as required
                return View::dispatchTouchUp(content,touchX,touchY,pointerId,false);
            }
        }break;
        case EMotionEventAction::UP:
        {
            KSLOGD("KSEVENT", " Action up Id - %d, index %d",pointerId,pointerIndex);

            //Consider hover enter exit TODO
            if(content->isPointInside(touchX,touchY) )//|| View::isHandlingTouch(content, pointerId) )
            {
                //TODO later avoid or do as required
                return View::dispatchTouchUp(content,touchX,touchY,pointerId,true);
            }else
            {
                KSLOGW("KSEVENT", "Action up warning");//Touch up outside content view/should already be handle in move,as hoverExit

            }

        }break;

        default:
            assert(false);//TODO

    }
    return false;
}

bool KSApplication::onInterceptKeyEvent(const KeyEvent &ke) {
    if(ke.getKeyAction() == EKeyAction::KEY_DOWN)
    {

        if(ke.getKeyCode() == EKeyCode::KS_KEY_CODE_SHIFT_LEFT)
        {
            bIsCapsOn = true;
        }

        for(auto l : keyboardListeners)
        {
            if(l)l->onInterceptKeyEvent(ke);
        }
    }else if(ke.getKeyAction() == EKeyAction::KEY_UP)
    {
        if(ke.getKeyCode() == EKeyCode::KS_KEY_CODE_SHIFT_LEFT)
        {
            bIsCapsOn = false;
        }
    }

    return false;
}


void KSApplication::addKeyboardListener(ks::KeyEventInterceptor *l) {
    for(auto k : keyboardListeners)
    {
        if(k ==l)
        {
            KSLOGD(appName.c_str(),"Keyboard Listener already exist %p",l);
            return;
        }
    }
    keyboardListeners.push_back(l);

}

void KSApplication::removeKeyboardListener(ks::KeyEventInterceptor *l) {

   for(auto iter = keyboardListeners.begin(); iter != keyboardListeners.end();iter++)
   {
       if(*iter == l)
       {
           keyboardListeners.erase(iter);
           return;
       }
   }

    KSLOGW(appName.c_str(),"cannot remove keyboardListener doesn't exist %p",l);

}


//Static Memebers

KSImage* KSApplication:: _loadImageAsset(const char *path)
{
    return AppJavaCalls::loadImageAsset(path);

}

void KSApplication::setKeyEventIntercepor(ks::KeyEventInterceptor *i) {

    AndroidEvents::setKeyEventInterceptor(i);

}

void KSApplication::setMotionEventInterceptor(ks::MotionEventInterceptor *i) {
    AndroidEvents::setMotionEventInterceptor(i);
}

const std::string KSApplication::_getExternalStoragePath() const {
    return app->activity->externalDataPath;
}

const std::string KSApplication::_getInternalStoragePath() const {
    return app->activity->internalDataPath;
}

const std::string KSApplication::_getOBBPath() const {
    return app->activity->obbPath;
}

void KSApplication::getFileListInDir(const char *directory,std::vector<std::pair<std::string, std::string>> &filePaths) {

    AAssetDir* dir = AAssetManager_openDir(getAssetManager(),directory);
    const char* fileName;
    if(dir != nullptr)
    {
        while((fileName = AAssetDir_getNextFileName(dir)) != nullptr)
        {
            filePaths.push_back({fileName,""});
        }

        AAssetDir_close(dir);
    }else
    {
        KSLOGE(appName.c_str(),"Coundn't open Asset Dir");
    }

}

bool KSApplication::_copyAssetDirToDevice(const char *assetDir, const char *destination) {

    AAssetDir* dir = AAssetManager_openDir(getAssetManager(),assetDir);
    const char* fileName;
    if(dir != nullptr)
    {
        while((fileName = AAssetDir_getNextFileName(dir)) != nullptr)
        {
            std::string folderFile = std::string(destination) + "/" + fileName;
            std::string assetPath = std::string(assetDir) + "/" + fileName;

            KSLOGD(appName.c_str(),"Copying assets dir %s to internal storage %s, path %s to path %s",assetDir,destination,folderFile.c_str(),assetPath.c_str());

            KSAssetReader *reader = dynamic_cast<KSAssetReader *>(openAsset(assetPath.c_str()));
            if(reader == nullptr)return false;//TODO only falure skip and continue with others;

            FDStream fdStream;
            fdStream.open(folderFile.c_str(),0777);

            int BUFSIZE = 1024;
            uint8_t buffer[BUFSIZ];
            int readSize = 0;

            do{
                readSize = reader->read(buffer,BUFSIZE);
                fdStream.write(buffer,readSize);
            }while(readSize == BUFSIZE);
            reader->close();
            delete reader;

        }

    }else
    {
        KSLOGD(appName.c_str(),"Open AssetDir failed %s",assetDir);
    }
    return true;
}

KSImage *KSApplication::_loadImage(const char *path) {
    return AppJavaCalls::loadImageFile(path);

}

AppEventMonitor *KSApplication::getAppEventMonitor() {
    return appEvents;
}

void KSApplication::onAppEvent(AppEvent event) {

    KSLOGD(appName.c_str(),"Unhandled App event");
}


AssetManager* AssetManager::mAssetManager = nullptr;
FileManager* FileManager::fileManager = nullptr;
bool  KeyboardController::bKeyboardOpen = false;
bool KeyboardController::closeKeyboard() {

    if(bKeyboardOpen)
    {
        AppUtils::toggleKeyboardOpen();//TODO
        bKeyboardOpen = false;
    }

    return false;
}

bool KeyboardController::openKeyboard() {
    if(!bKeyboardOpen)
    {
        AppUtils::toggleKeyboardOpen();//TODO make sure the keyboard is on;
        bKeyboardOpen = true;
    }

    return false;
}
