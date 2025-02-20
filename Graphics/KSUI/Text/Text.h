//
// Created by pspr1 on 10/24/2020.
//

#ifndef DIGIBAND_TEXT_H
#define DIGIBAND_TEXT_H

#include <Display.h>
#include <string>
#include <CMedia/KSImage.h>
#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "android/log.h"
#include "Commons.h"

//TODO REfactor whole thing also include Kerning
//TODO use common bitmap structs
typedef int status;
struct PixaMap{
    uint8_t *pixels = nullptr;
    int width,height;
};

void textSetup();
void stringToPixmap(const char* string, PixaMap *stringMap);
class TextEngine{

public:


    static TextEngine* getInstance(){

        if(!textEngine)
        {
            textEngine = new TextEngine;
            textEngine->init();
        }
        return textEngine;
    }


    //WIDTH AND HEIGHT IN POINTS(1/72 th of inch)
    status setTextSize(int  width,int height);

    status setFont(const char* fontFile);

    void getPixamapFromString(const std::string &pString,PixaMap *pixaMap);

    void setDisplayMetrics(const DisplayMetrics &displayMetrics){displayParams = displayMetrics;}

    KSImage *getImageFromText(const std::string &pString);

private:

    TextEngine() {};

    status init();



private:

     FT_Library ftLibrary;//handle to the instance of Library//make static if need only one for entire app.

     FT_Face fontFace;

     int fontHeight = 28,fontWidth = 28;

     const char *defaultFont="/system/fonts/DroidSans.ttf";

     DisplayMetrics displayParams;

private:

    static TextEngine *textEngine ;


};

#endif //DIGIBAND_TEXT_H
