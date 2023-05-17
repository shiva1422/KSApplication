
//
// Created by kalasoft on 1/8/22.
//

#include "KSAssetReader.h"
#include "stddef.h"

//TODO inline all
KSAssetReader::KSAssetReader(AAsset *asset) {

    this->asset = asset;
    setSource(this->asset);
}

KSAssetReader::~KSAssetReader() {

    AAsset_close(asset);
}

int KSAssetReader::read(void *buf, size_t count) {
    return AAsset_read(asset,buf,count);
}

off_t KSAssetReader::seek(off_t offset, int whence)
{
    return AAsset_seek(asset,offset,whence);
}

int KSAssetReader::close() {
     AAsset_close(asset);
    return 0;
}

int KSAssetReader::open(const char *path, int flags) {
    return 0;
}

long KSAssetReader::getSize() {
    return AAsset_getLength64(asset);
}

int KSAssetReader::eof()
{
    return AAsset_getRemainingLength64(asset) <= 0 ? 1 : 0;
}

 int64_t KSAssetReader::getCurrentPosition()
{
    return (AAsset_getLength64(asset) - AAsset_getRemainingLength64(asset));//can also seek(0)
}