//
// Created by kalasoft on 3/5/22.
//

#ifndef KALASOFT_VIEWGROUP_H
#define KALASOFT_VIEWGROUP_H

#include "View.h"
#include<vector>
#include <KSUI/GL/GLView.h>

//TODO Merge this into View, no explicit class required;
class ViewGroup : public GLView{

public:

    ViewGroup();

   virtual ~ViewGroup() override;

    virtual void addView(View * child);

    virtual void removeView(View *view);

    //add View at specified offset position  (startXOffset = child->startX - this.getStartX) similar for y
    //virtual void addView(View* child,float startXOffset,float starYOffset) = 0;

    //get View At position inside this ViewGroup
   // virtual View* getViewAtPosition(float x,float y) = 0;

    virtual void draw() override;

    void setBounds(float startX, float startY, float width, float height) override;

    void setBounds(float width, float height) override;

    void setXY(int x, int y) override;

    void setGradient(float r1, float g1, float b1, float r2, float g2, float b2,
                     float gradientStrength) override;

    View* getViewContainingPoint(float x,float y);

    void removeAllViews(){children.clear();}


protected:

    /**
     * @return total count of the views/children in this view group;
     */
    int getViewCount()const{return children.size();};

    /**
     * @param child : the child that will be brought front of the view that is just above it
     */
    void bringChildToFront(View *child);

    /**
     *
     * @param child : the child that will be sent back of the view that is just below it.
     */
    void sendChildToBack(View *child);


protected:

//the index is considered the z-order; lower index means at the bottom; //Can also construct and 2D graph if the need comes;

    std::vector<View *> children;

    friend class ViewGroupTouchListener;

    GLView glBounds;//Bounds of this View

};


class ViewGroupTouchListener : public View::TouchListener{

protected:

    ~ViewGroupTouchListener() override;

    bool onTouch(const ks::MotionEvent &event, View *view) override;

    /**
     * @brief :  Dispatch touch to the first view(in the LIFO order the views were added as children ) that contains (x,y)  within its bounds, if that view doesn't handle touch down, the touch down events are not navigated to the views below it and if required the functionality needs to be added here.
     * @return : returns true if  any child has handled the touch down and further events with the same TouchID will be navigated to that view only if it returned true;
     */
    bool onTouchDown(const float &x, const float &y, const ks::TouchID &id,
                     const bool &isPrimary) override;

    bool
    onTouchUp(const float &x, const float &y, const ks::TouchID &id, const bool &isLast) override;

    bool onMove(const float &x, const float &y, const ks::TouchID &id) override;

    bool onHoverExit(const ks::TouchID &id, const float &x, const float &y) override;

    bool onHoverEnter(const float &x, const float &y,const ks::TouchID &id)override;



private:

    std::unordered_map<ks::TouchID,View*> touchHandlingViews;

    //This is to store view that is touched but doesn't handle touch;//If needed later
    std::unordered_map<ks::TouchID,View*> touchNotHandlingViews;


};

#endif //KALASOFT_VIEWGROUP_H
