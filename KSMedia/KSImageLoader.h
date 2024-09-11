//
// Created by Shiva Pandiri on 5/20/2022.
//

#ifndef KALASOFT_KSIMAGELOADER_H
#define KALASOFT_KSIMAGELOADER_H

#include "ImageCodec/STBImageLoader.h"
#include "../../Core/CMedia/KSImage.h"

class KSImageLoader {
    
public:
    
    static KSImage loadFromAsset(const char* path);
};


#endif //KALASOFT_KSIMAGELOADER_H
