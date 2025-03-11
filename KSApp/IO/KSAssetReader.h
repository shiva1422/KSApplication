

//
// Created by kalasoft on 1/8/22.
//

#ifndef KALASOFT_KSASSETREADER_H
#define KALASOFT_KSASSETREADER_H


#include <KSIO/IKSStream.h>
#include <cstdio>
#include <android/asset_manager.h>

//only for android app
class AssetManager;
class KSAssetReader : public IKSStream{

public:

    KSAssetReader() = delete;

    KSAssetReader(AAsset *asset);

    ~KSAssetReader();

    int read(void *buf, size_t count) override;

    off_t seek(off_t offset, int whence) override;

    int open(const char *path, int flags) override;

    int close() override;

    int eof() override;

    long getSize() override;

    int64_t getCurrentPosition()  override;

private:

    AAsset *asset = nullptr;
    //create reader only through assetManager;
    friend  AssetManager;


};



#endif //KALASOFT_KSASSETREADER_H
