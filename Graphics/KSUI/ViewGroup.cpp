//
// Created by kalasoft on 3/5/22.
//

#include "ViewGroup.h"


ViewGroup::~ViewGroup(){
    //TODO
}
ViewGroup::ViewGroup() {

    this->setTouchListener(new ViewGroupTouchListener);
}

void ViewGroup::addView(View *child) {

    children.push_back(child);
    child->parent = this;
}


void ViewGroup::bringChildToFront(View *child) {


    for(int i = 0 ; i < children.size(); ++i)
    {
        if(child == children[i] && (children.size() > 1 && i < (children.size()-1)))
        {
            children[i] = children[i+1];
            children[i+1] = child;
            return;
        }
    }

    KSLOGW("ViewGroup", "couldn't bring child to front");
}

void ViewGroup::sendChildToBack(View *child) {

    for(int i = 0 ; i < children.size(); ++i)
    {
        if(child == children[i] && (children.size() > 1))
        {
            children[i] = children[i-1];
            children[i-1] = child;
            return;
        }
    }

    KSLOGW("ViewGroup", "couldn't send child to back");
}

void ViewGroup::removeView(View *view) {

    for(auto iter = children.begin(); iter != children.end(); iter++)
    {
       if(*iter == view)
       {
           view->parent = nullptr;
           children.erase(iter);//TODO destroy check.
           return;
       }
    }

    KSLOGW("ViewGroup", " couldn't remove the view");
}

void ViewGroup::setBounds(float startX, float startY, float width, float height) {
    View::setBounds(startX, startY, width, height);
    //TODO childrenViews
}

void ViewGroup::setBounds(float width, float height) {
    View::setBounds(width, height);
    //TODO childrenViews

}

void ViewGroup::setXY(int x, int y) {
    View::setXY(x, y);
    //TODO childrenViews

}

void ViewGroup::setGradient(float r1, float g1, float b1, float r2, float g2, float b2,
                            float gradientStrength) {
    View::setGradient(r1, g1, b1, r2, g2, b2, gradientStrength);
    //TODO childrenViews

}

void ViewGroup::clearBackground() {

    //TODO
}

void ViewGroup::draw() {

    for(int i =0; i < children.size() ; i++)
    {
        this->children[i]->draw();
    }
}



//TouchListener
ViewGroupTouchListener::~ViewGroupTouchListener() {

}

bool ViewGroupTouchListener::onTouch(const ks::MotionEvent &event, View *view) {
    return false;
}

bool ViewGroupTouchListener::onTouchDown(const float &x, const float &y, const ks::TouchID &id,
                                         const bool &isPrimary) {
    ViewGroup *viewGroup = (ViewGroup *)(this->view);
    if(viewGroup)
    {

        for(int i = viewGroup->getViewCount()-1; i > -1; i--)
        {
           if(viewGroup->children[i]->isPointInside(x,y))
           {

               if(View::dispatchTouchDown(viewGroup->children[i],x,y,id,isPrimary))
               {
                   touchHandlingViews.insert({id,viewGroup->children[i]});
                   return true;
               }else
               {
                   touchNotHandlingViews.insert({id,viewGroup->children[i]});//TODO if not use can be removed
                   return false;//Todo decide if navigate event/following events to views underneath.
               }

           }
        }
    }
    return false;

}

bool ViewGroupTouchListener::onTouchUp(const float &x, const float &y, const ks::TouchID &id,
                                       const bool &isLast) {

    //Check if there is a handler for this ID and if exist dispatch touch up;
    ViewGroup *viewGroup = (ViewGroup *)(this->view);

    View *handlerView = nullptr;
    auto iter = touchHandlingViews.find(id);
    if(iter != touchHandlingViews.end()) {
        handlerView = iter->second;
    }
    if(handlerView)
    {
        View::dispatchTouchUp(handlerView,x,y,id,isLast);
        touchHandlingViews.erase(id);
        if(isLast)touchHandlingViews.clear();//Should trigger actually;
    }


    return false;
}

bool ViewGroupTouchListener::onMove(const float &x, const float &y, const ks::TouchID &id) {

    ViewGroup *viewGroup = dynamic_cast<ViewGroup*>(this->view);
    if(viewGroup)
    {
        //1.find if there is already a view handling TouchID,
        //2.if there is check if x,y inside that view;.(True || false);
        //3. if (true) dispatch move to that view and return;
        //4. else dispatch hover exit to that view and remove that pair from the touchandlingviews;
        //5. find the view that x,y is inside and dispatch hover enter to that view;

        View *handlerView = nullptr;
        auto iter = touchHandlingViews.find(id);
        if(iter != touchHandlingViews.end())
        {
            handlerView = iter->second;
        }
        if(handlerView) //there should be if touch down and up or hover enter previously
        {

            if(handlerView->isPointInside(x,y))//TODO || handlerView->isMonitoring(touchOuside).
            {
                KSLOGD("CLICK","Handler view exist and point inside that view");
                bool res = View::dispatchMove(handlerView,x,y,id);//if false should pair be removed?
                if(!res)
                {
                    touchHandlingViews.erase(id);//noTouchHandlingInsert
                }
                return res;
            }
            else
            {
                KSLOGD("CLICK","Handler view exist and point outside that view that view");
                bool res = View::dispatchHoverExit(handlerView,x,y,id);//This is also dragging outside
                if(res)
                {
                    //the view still handles touch outside  its bounds
                    return true;
                }else
                {
                    //trigger hoverenter to other views;


                    for(int i = viewGroup->getViewCount()-1; i > -1; i--)
                    {
                        if( viewGroup->children[i] != handlerView && viewGroup->children[i]->isPointInside(x,y))
                        {
                           if(View::dispatchHoverEnter(viewGroup->children[i],x,y,id))
                           {
                               touchHandlingViews[id] = viewGroup->children[i];
                               return res;
                            }else
                            {
                                break;
                                //TODO add logic not dispatch hover enter agian to this view;
                            }

                        }
                    }

                    touchHandlingViews.erase(id);
                    return false;

                }
            }
        }
        else
        {
            KSLOGE("Click","ViewGroup , no handler view");
        }

    }else
    {
        KSLOGE("ViewGroupTouch", "wrong touch handler set");

    }
    return false;
}

bool ViewGroupTouchListener::onHoverExit(const ks::TouchID &id, const float &x, const float &y) {

    KSLOGD("ViewGroupTouchListener", "  onHoverExit");
    ViewGroup *viewGroup = (ViewGroup *)(this->view);

    View *handlerView = nullptr;
    auto iter = touchHandlingViews.find(id);
    if(iter != touchHandlingViews.end()) {
        handlerView = iter->second;
    }
    if(handlerView)
    {
        bool res = View::dispatchHoverExit(handlerView,x,y,id);
        if(!res)
        {
            touchHandlingViews.erase(id);
        }
        return res;
    }

    return TouchListener::onHoverExit(id,x, y);
    //cancel touch;
}

bool ViewGroupTouchListener::onHoverEnter(const float &x, const float &y, const ks::TouchID &id) {

    KSLOGD("ViewGroupTouchListener", "onHoverEnter");//FORWARD TO the Views
    return TouchListener::onHoverEnter(x, y, id);
}
