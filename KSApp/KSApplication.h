//
// Created by kalasoft on 10/5/21.
//

#ifndef KALASOFT_KSAPPLICATION_H
#define KALASOFT_KSAPPLICATION_H

#include <android_native_app_glue.h>
#include <Graphics/GLContext.h>
#include <Graphics/Display.h>
#include <Graphics/KSUI/Renderer/GLUIRenderer.h>
#include <KSIO/IKSStream.h>
#include <Graphics/KSUI/Renderer/Renderer.h>
#include <KSIO/AssetManager.h>
#include <string>
#include "Events/MotionEvent.h"
#include "Events/KeyEvent.h"

/*
 * TODO
 * Rendering ,Now renderer has a window instead better way to abstract Window to have renderers
 */

class View;
class AndroidEvents;
class CustomEvents;

class KSApplication : public AssetManager , public ks::MotionEventInterceptor, public ks::KeyEventInterceptor{

public:

    KSApplication() = delete;

    //TODO make paap as opaque for different platforms
    KSApplication(android_app *papp,std::string name = "KSApplication") ;

    ~KSApplication();
    //friends
    friend AndroidEvents;

    friend CustomEvents;

public:

    //Process UIEvent,Draw to Screen and runTasks()
    void run();

    //Set App UI View
    void setContentView(const View *content);

    AAssetManager *getAssetManager();

    int getAssetFD(const char* assetLoc);

    IKSStream* _openAsset(const char* assetPath) override;

    //getCurrent UI Content
    View* getContentView() const;

    DisplayMetrics getDisplayMetrics(){return displayMetrics;}


public:

    void setKeyEventIntercepor(ks::KeyEventInterceptor *i);

    void setMotionEventInterceptor(ks::MotionEventInterceptor *i);

    bool onInterceptMotionEvent(const ks::MotionEvent& me)  override;

    bool onInterceptKeyEvent(const ks::KeyEvent &ke) override;



protected:
    //lifeCycle and Events

    //createUI and Listeners and set UI View in Renderer by calling setContentView()
    virtual void onCreate();

    virtual void onStart();

    virtual void onResume();

    virtual void onPause();

    virtual void onStop();

    virtual void onDestroy();

    virtual void setWindow(ANativeWindow *window){this->window.setWindow(window);}

    //TODO pass window from android events as param to window methods.
    virtual void onWindowInit();

    KSWindow getWindow(){return window;}

    virtual void onWindowResized();

    virtual void onWindowRedrawNeeded();

    virtual void onWindowTermination();

    virtual void onContentRectChanged();

    virtual void onLowMemory();

    virtual void onFocusLost();

    virtual void onFocusGained();

    virtual void onConfigChanged();

    virtual void onSaveState();

    virtual void onInputChange();

    void onScreenRotation();




//Event Loop
protected:


    //tasks to run every frame//TODO rename use apt queue looper
    virtual void runTasks(){};

    //drawing UI ,make sure Super method is called when implementing this function;
    virtual void onDraw();
    //This is to respect both displaymetrics and ANativeWindowSize,be careful while using
    void forceUpdateDisplayMetrics(DisplayMetrics &displayMetrics);


protected:

    AAsset *getAsset(const char* assetPath);

    KSImage* _loadImageAsset(const char *path) override;



protected:


    DisplayMetrics displayMetrics{0};

    //All UI and display related control only through renderer(even dispMetrics and view initalizations)
     Renderer *renderer;//todo clear resources.

     std::string appName{"KSApplication"};

     bool bWindowInit = false;


private:

    bool updateDisplayMetrics();

private:

    android_app *app;

    //UIRenderer
    GLContext glContext;
    KSWindow window;

    CustomEvents *customEvents;////////////

    //VulkanContext vulkanContext;
    // GLuint uiProgram;
    bool bGraphicsInit = false;
    bool bAppFirstOpen = true;
    bool bAppDestroyed = false, bUseGL = true;//TODO bUseGL

    int screenOrientation  = ACONFIGURATION_ORIENTATION;

    //Activity call backs;Protected?????

    //onCreate is the main method;

    std::chrono::time_point<std::chrono::system_clock> frameClock;
    std::chrono::time_point<std::chrono::system_clock> previousFrameClock;




};




#endif //KALASOFT_KSAPPLICATION_H
