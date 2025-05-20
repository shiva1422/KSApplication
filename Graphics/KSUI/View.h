//
// Created by kalasoft on 10/16/21.
//

#ifndef KALASOFT_VIEW_H
#define KALASOFT_VIEW_H

#include "Logger/KSLog.h"
#include "KSApp/Events/MotionEvent.h"
#include "unordered_set"
#include "Renderer/Renderer.h"

//TODO DISPLAYMETRIC NO NEED DECIDE ACTUAL PARAMS BEFORE SETTING BOUNDS TO VIEW
//TODO Merge ViewGroup into this.


    class DisplayMetrics;
    class ClickListener;
    class View {

    public:

        class TouchListener{

            friend class View;

        protected:

            View *view ;

            virtual ~TouchListener(){};

            //return true if event is handled and further events to be triggered //TODO if required later event to pointer(MotionEvent *) as well
            virtual bool onTouch(const ks::MotionEvent &event, View* view) = 0;

            //override and return true if handling touch and need further events to come in,TODO later passView* as param
            virtual bool onTouchDown(const float &x, const float &y, const ks::TouchID &id,const bool &isPrimary) {return false;}

            virtual bool onTouchUp(const float &x, const float &y, const ks::TouchID &id, const bool &isLast){return false;};

            virtual bool onMove(const float &x, const float &y, const ks::TouchID &id/* , isHover?*/){return false;};

            virtual bool onHoverExit(const ks::TouchID &id, const float &x, const float &y) {return false;}

            virtual bool onHoverEnter(const float &d, const float &d1, const ks::TouchID &i) {return false;}

            KSFORCEINLINE bool isHandlingTouch(const ks::TouchID &id){return touchIds.find(id) != touchIds.end();}


        private:

            //Touch Id's currently handled by this listener,TODO may be associate with motion action as well
            std::unordered_set<ks::TouchID> touchIds;



        };

    public:

        //Generally set startX,startY in INTS, and width and height in Floats
        View();

        //float as subPixel precisions touch,try to set width and height in int pixels
        View(float width, float height);


        View(float startX, float startY, float width, float height);

        virtual ~View();

        //bounds set in int pixels using float for subpixel precision/touch handling
        virtual void setBounds(float startX, float startY, float width, float height);

        virtual void setBounds(float width, float height);

        virtual void setBoundsWithCentre(float centreX,float centreY,float width,float height);


        virtual void setXY(int x,int y){setBounds(x,y,width,height);}

        float getStartX()const{return startX;}

        float getStartY()const{return startY;}

        float getWidth()const{return width;}

        float getHeight()const{return height;}

        float getEndX()const{return startX + width;}

        float getEndY()const{return startY + height;}

        //TODO accurate //odd Pixel,touch
        float getCentreX()const{return startX + width/2.0;}

        float getCentreY()const{return startY + height / 2.0;}

        //TODO add below ints to floats;

        //fit the view inside a box keeping the aspect ratio.
        void fitToBounds(int BStartX,int BStartY,int BWidth,int BHeight,bool keepAspectRation = true);

        void fitToBoundsWithCentre(int centreX,int centreY,int BWidth,int BHeight);

        /**
         * @brief check if point (x,y) lies withing the bounds of this view
         */
        bool isPointInside(float x,float y) const
        {
            return ( x >= startX && x <= (startX + width) && y >= startY && y <= (startY +height));
        }


        //color & drawing
        void setAlpha(float alpha){this->a = alpha;}

        void setBackgroundColor(float red,float green,float blue,float alpha);

        virtual void setGradient(float r1,float g1,float b1,float r2,float g2,float b2,float gradientStrength){};

        virtual void clearBackground() = 0;

        //TODO change ACCESS?;
        virtual void draw() = 0;


        View* getParent(){return this->parent;}

        void removeFromParent();

        View* getRootView();


        //TOUCH

        void setTouchListener(TouchListener *pTouchListener)
        {
          delete this->touchListener;
          this->touchListener = pTouchListener;
          this->touchListener->view = this;
        }

        void setClickListener(ClickListener *clickListener);

        KSFORCEINLINE TouchListener* getTouchListener(){return touchListener;}

        //Later modify dispatching as required
        KSFORCEINLINE static bool dispatchTouchEvent(View *view ,const ks::MotionEvent &event )
        {
            return  (view->touchListener && view->touchListener->onTouch(event,view));
        }

        KSFORCEINLINE static bool dispatchTouchDown(const View * view,const float &x,const float &y,const ks::TouchID &id,const bool &isPrimary)
        {
            bool res =  (view->touchListener && view->touchListener->onTouchDown(x,y,id,isPrimary));
            if(res){view->touchListener->touchIds.insert(id);}
            return res;
        }

        KSFORCEINLINE static bool dispatchTouchUp(const View * view,const float &x,const float &y,const ks::TouchID &id,const bool &isLast)
        {
            bool res =  (view->touchListener && view->touchListener->isHandlingTouch(id) && view->touchListener->onTouchUp(x,y,id,isLast));
            if(view->touchListener)
            {
                if(isLast)
                    view->touchListener->touchIds.clear();
                else
                view->touchListener->touchIds.erase(id);

            }

            return res;
        }

        KSFORCEINLINE static bool dispatchMove(const View *view,const float &x,const float &y,const ks::TouchID &id)
        {
            bool res = false;
            if(view->touchListener)
            {
                 res =  view->touchListener->onMove(x,y,id);
                 if(!res)
                 {
                     view->touchListener->touchIds.erase(id);
                 }
            }
            return res;
        }

        KSFORCEINLINE static bool dispatchHoverExit(const View* view,const float &x,const float &y,const ks::TouchID &id)
        {
            bool res = false;
            if(view->touchListener)
            {
                res = view->touchListener->onHoverExit(id, x, y);
                if(!res)
                    view->touchListener->touchIds.erase(id);
            }
           //TODO check apt if hoverEnter is acceptble
            return res;

        }
        KSFORCEINLINE static bool dispatchHoverEnter(const View* view,const float &x,const float &y,const ks::TouchID &id)
        {
            bool res = false;
            if(view->touchListener)
            {
                 res =  view->touchListener->onHoverEnter(x,y,id);
                if(res)
                    view->touchListener->touchIds.insert(id);//TODO check apt
            }
            return res;
        }

        KSFORCEINLINE static bool isHandlingTouch(const View *view, const ks::TouchID &id)
        {
            return (view->touchListener && view->touchListener->isHandlingTouch(id));
        }

        Renderer* getRenderer();

        void setRenderer(Renderer *prenderer){this->renderer = prenderer;}



        //virtual void onScreenRotation();

    public:

        friend class Renderer;
        friend class ViewGroup;

        bool bDrawBackGround = false;

    protected:

        View *child = nullptr;

        static DisplayMetrics dispMetrics;

        Renderer *renderer = nullptr;

        float startX = 0,startY = 0,width = 50,height = 50;

        float r = 0.5f,g = 0.5f,b = 0.5f,a = 0.0f;

        bool bVisible = true;


        TouchListener *touchListener= nullptr;


        View *parent = nullptr;
    };




#endif //KALASOFT_VIEW_H
