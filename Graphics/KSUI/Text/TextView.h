//
// Created by "SemmGod(Shiva Shanker Pandiri)" on 11/24/24.
//

#ifndef KALASOFT_TEXTVIEW_H
#define KALASOFT_TEXTVIEW_H

#include <KSUI/GL/GLView.h>
#include "Text.h"
#include <CMedia/KSImage.h>
#include <KSUI/GL/GLImageView.h>


//TODO map all characters into a Texture and draw based on index;
class TextView : public GLView{

public:

    TextView();

    virtual ~TextView();

    void clearBackground() override;

    void setBounds(float startX, float startY, float width, float height) override;

    void setBounds(float width, float height) override;

    void draw() override;

public:

    //TODO later align with Bounds with Alignment
    void alignTextToCentre();


    void setText(const std::string &t);

    std::string getText(){return text;}



private:

    std::string text;
    KSImage *textImage = nullptr;
    GLImageView textImageView;
    uint fontSize = 14 ;

    bool alignCentre = false;//Later add enums and process accordingly;


public:

};


#endif //KALASOFT_TEXTVIEW_H
