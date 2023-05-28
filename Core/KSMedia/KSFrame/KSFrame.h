//
// Created by Shiva Pandiri on 12/31/21.
//

#ifndef KALASOFT_KSFRAME_H
#define KALASOFT_KSFRAME_H


#include <Commons.h>
#include "unordered_map"
#include "string"
//vDes


class FrameBuf{
    
    uint8_t *data;
}
class KSFrame {

public:

    KSFrame(){}

   
  
    KSFORCEINLINE void setPrivData(std::string key,int val)
    {
        privData.insert({key,val});
    }
   
    FrameBuf *getBuffer(){return buf;};
    
    KSFORCEINLINE std::unordered_map<std::string,int> getPrivData(){return privData;}

public:
    
protected:
    
    std::unordered_map<std::string,int> privData;
    FrameBuf *buf;
   

};


#endif //KALASOFT_KSFRAME_H
