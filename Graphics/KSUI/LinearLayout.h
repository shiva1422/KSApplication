//
// Created by kalasoft on 3/5/22.
//

#ifndef KALASOFT_LINEARLAYOUT_H
#define KALASOFT_LINEARLAYOUT_H


#include "ViewGroup.h"
#include "vector"

enum ELayoutDirection{HORIZONTAL,VERTICAL};

class LinearLayout : public ViewGroup{

public:

     LinearLayout();

    ~LinearLayout() ;

    void setBounds(float startX, float startY, float width, float height) override;

    void clearBackground() override;

    void draw() override;

    void addView(View *child) override;

    void setBounds(float width, float height) override;


private:
    //Apt vector? constant check;
    //keep index of lastView outofbounds so as not to draw after that;

    std::vector<View *> children;

    ELayoutDirection layoutDirection = VERTICAL;


    int gapY;
};


#endif //KALASOFT_LINEARLAYOUT_H
