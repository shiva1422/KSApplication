//
// Created by Shiva Shanker Pandiri on 3/12/25.
//

#include "Font.h"

const char* Font::MY_TYPE;
std::string Font::FONT_DIR;


void Font::initFont(const std::string &fontDir) {

    FONT_DIR = fontDir;
    MY_TYPE  = (FONT_DIR +  "/" +"mytype.ttf").c_str();
}
