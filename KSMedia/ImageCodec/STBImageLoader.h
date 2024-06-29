//
// Created by Shiva Pandiri on 5/21/2022.
//

#ifndef KALASOFT_STBIMAGELOADER_H
#define KALASOFT_STBIMAGELOADER_H

//TODO stb_image.h to out of project?
#include "CMedia/KSImage.h"

class STBImageLoader {
    
public:
    
    static void fromAsset(const char* path, KSImage &image);

};


#endif //KALASOFT_STBIMAGELOADER_H
