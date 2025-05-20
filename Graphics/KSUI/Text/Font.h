//
// Created by Shiva Shanker Pandiri on 3/12/25.
//

#ifndef KALASOFT_FONT_H
#define KALASOFT_FONT_H

#include<string>

class Font {

public:

    static void initFont(const std::string &fontDir);

    static const char* MY_TYPE;

private:

    static std::string FONT_DIR;

};


#endif //KALASOFT_FONT_H
