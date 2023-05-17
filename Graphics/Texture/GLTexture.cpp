//
// Created by Shiva Pandiri on 5/22/2022.
//

#include "string"
#include <Logger/KSLog.h>
#include <string>
#include "GLTexture.h"
//TODO different pixFMT // also directly from asset/file or any other sources
#define TAGLOG "GLTexture"
GLTexture::GLTexture(KSImage image)
{
    if(image.isValid())
    {
       create(image);
    }
    else
    {
        KSLog::error("GLTexture()","invalid image");
    }

    //TODO DELETE Image; but glBufferdata is async?
}

GLTexture::~GLTexture() {
        //TODO
}

void GLTexture::create(KSImage &image)
{
    //TODO does need pbo always?
    //TODO if recreate need not delete texure and buf if same dimensions and pixelfmt
    //also check if glMapBufferRange better
    width = image.width,height = image.height;
    if(glIsBuffer(pbo))
        glDeleteBuffers(1,&pbo);
    glGenBuffers(1,&pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    //TODO size and format convert if needed
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4, image.data, GL_STATIC_COPY);

    if(glIsTexture(tex))
        glDeleteTextures(1,&tex);
    glGenTextures(1,&tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
    //ERRORSS?
}

void GLTexture::reset(KSFrame *frame,bool bFreeFrame)
{

    //TODO rewrite;
  //TODO temp
  GLContext::getError("reset texture0");
  std::unordered_map<std::string,int> frameInfo = frame->getPrivData();
  int h = frameInfo.find("height")->second ;
  int w = frameInfo.find("width")->second;


  if(h != height || w != width)
  {
      //TODO can't resize memory without recreate texture?
      KSLog::warn(TAGLOG,"resizing texture");
      if(glIsTexture(tex))
          glDeleteTextures(1,&tex);
      if(glIsBuffer(pbo))
          glDeleteBuffers(1,&pbo);

      glGenBuffers(1,&pbo);
      glGenTextures(1,&tex);



   width = w;height = h;
  }

    GLContext::getError("reset texture1");


    //TODO size and format convert if needed
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4, frame->getBuffer()->data[0], GL_STATIC_COPY);
    GLContext::getError("reset texture11");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLContext::getError("reset texture5");

    //TODO no need to rcreate texture memory
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    GLContext::getError("reset texture6");


    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);

    GLContext::getError("reset texture4");

    //TODO decide aysnc PBO still required buffer can delete after first draw?
    if(bFreeFrame)
    delete frame->getBuffer();
}
