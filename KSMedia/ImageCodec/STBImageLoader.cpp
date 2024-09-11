//
// Created by Shiva Pandiri on 5/21/2022.
//

#include "new"
#include <KSIO/IKSStream.h>
#include <CMedia/KSImage.h>

extern "C"
{
  #define STB_IMAGE_IMPLEMENTATION
  #include "stb_image.h"
}
#include "STBImageLoader.h"
#include "Logger/KSLog.h"
#include "KSIO/AssetManager.h"

#define LOGTAG "STBImage"

stbi_io_callbacks assetIOCallbacks ={.read = reinterpret_cast<int (*)(void *, char *,int)>(CustomIO::read),.skip=reinterpret_cast<void (*)(void *, int)>(CustomIO::skip),.eof = CustomIO::eof};

//TODO return bool
void STBImageLoader::fromAsset(const char *path,KSImage &image)
{
    //TODO desired channels,setting all params of image correctly and set PIXEL format accordingly
    IKSStream *stream = AssetManager::openAsset(path);
    if(stream)
    {
        //stbi_set_flip_vertically_on_load(true);
        stbi_info_from_callbacks(&assetIOCallbacks, stream, &image.width, &image.height, &image.chCnt);
        stream->seek(0,SEEK_SET);
        image.data = stbi_load_from_callbacks(&assetIOCallbacks,stream, &image.width, &image.height, &image.chCnt, 4);
        stream->close();
        delete stream;
        stream = nullptr;
        if(image.data == NULL)
        {
            KSLOGE(LOGTAG,"%s KS_DECODE ERROR",path);
            return;
        }
        //TODO other formats if based on STB
        image.pixelFmt = PIXELFMT_RGBA8;
    }
    else
        KSLOGE(LOGTAG,"couldn't open asset");
}


/*
 * TODO load from memory
 * // Read the file:
    AAsset* file = AAssetManager_open(androidAppCtx->activity->assetManager,
                                      filePath, AASSET_MODE_BUFFER);
    size_t fileLength = AAsset_getLength(file);
    stbi_uc* fileContent = new unsigned char[fileLength];
    AAsset_read(file, fileContent, fileLength);
    AAsset_close(file);

    uint32_t imgWidth, imgHeight, n;
    unsigned char* imageData = stbi_load_from_memory(
            fileContent, fileLength, reinterpret_cast<int*>(&imgWidth),
            reinterpret_cast<int*>(&imgHeight), reinterpret_cast<int*>(&n), 4);
 */