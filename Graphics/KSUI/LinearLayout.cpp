//
// Created by kalasoft on 3/5/22.
//

#include <Logger/KSLog.h>
#include "LinearLayout.h"
#define TAG "LinearLayout"


LinearLayout::LinearLayout()
{
  KSLog::debug(TAG,"Construct");
}

LinearLayout::~LinearLayout()
{

    //TODO delete only which are not deleted outsied(almost every view )
    for(auto & iter : children)
    {
       delete iter;
       iter = nullptr;
       //to remove iter? no need as its already default deleted;
    }

    KSLog::warn(TAG,"Check all properly deleted");

}

void LinearLayout::setBounds(float startX, float startY, float width, float height) {
    ViewGroup::setBounds(startX, startY, width, height);//childeren should adust accordingly;

    //Layout vertical todo horizontal
    int previousEndY = startY;
    for (auto child : children)
    {
        child->setBounds(startX,previousEndY + gapY,child->getWidth(),child->getHeight());
        previousEndY = child->getEndY();
    }

}

void LinearLayout::setBounds(float width, float height) {
    ViewGroup::setBounds(width, height);
}


void LinearLayout::clearBackground() {

}

void LinearLayout::draw() {

    //clipBounds();common here;
    clearBackground();
    for(auto view : children)
    {
        view->draw();
    }

}

void LinearLayout::addView(View *child) {

    //use different DS for children cause here linear search?
    if(child == nullptr)
        return;
    //TODO required?
    for(auto view : children)
    {
        if(view == child)
        {
            KSLog::warn(TAG,"already there");
            return;
        }
    }

    //TODO direction and Bounds
    float gapY;//TODo
    if(children.size() != 0)
    {
        auto lastView = children.back();
        child->setBounds( this->startX,lastView->getEndY() + gapY,child->getWidth(),child->getHeight());
    }
    else
    child->setBounds(this->startX,this->startY + gapY,child->getWidth(),child->getHeight());

    children.push_back(child);
}

