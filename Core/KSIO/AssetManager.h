//
// Created by kalasoft on 1/8/22.
//

#ifndef KALASOFT_ASSETMANAGER_H
#define KALASOFT_ASSETMANAGER_H

#include <Logger/KSLog.h>
#include "IKSStream.h"
class AssetManager {

public:

     //all open assets go through this
     static IKSStream* openAsset(const char* assetPath)
    {
        KSLog::debug("AssetManger","openAsset %s",assetPath);
        return mAssetManager ? mAssetManager->_openAsset(assetPath) : nullptr;
    }

protected:

    //defined in KSApplication.cpp
    static AssetManager *mAssetManager  ;

private:

    virtual IKSStream* _openAsset(const char* assetPath) = 0;

};


#endif //KALASOFT_ASSETMANAGER_H
