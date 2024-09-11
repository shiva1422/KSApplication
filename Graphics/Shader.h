//
// Created by kalasoft on 10/9/21.
//

#ifndef KALASOFT_SHADER_H
#define KALASOFT_SHADER_H

#include <KSIO/IKSStream.h>
#include "GLContext.h"


/*
 * Context should be current on this thread to create ShaderProgram.
 * TODO thread specific
 */

class AssetManager;

class Shader{


public:

    /**
     * @brief  \n Reads the streams ,creates a shader program .Streams are closed here;
     *
     * @return    shader program id or O(zero) if compile failed.
     */
    static GLuint createProgram(IKSStream *vertStr, IKSStream *fragStr);


    static GLuint createProgram(const char *vertSrc,const char *fragSrc);

public:

    static bool prepareShaders(AssetManager *assetManager);
    static GLuint getTextureProgram(){return texProg;}
    static GLuint getRectProgram(){return rectProg;}
    static GLuint getAlphaBlockProgram(){return alphaBlock;}


    static GLint  getTextureVertsLocation(){return textureVertsLocation;}//Move these to apporiate texture files
    static GLint  getTextureCoordsLocation(){return textureCoordsLocation;}
    static GLint  getRectVertsLocation(){return rectVertsLocation;}
    static GLint  getRectColorLocation(){return rectColorLocation;}
    static GLint  getAlphaBlockVertsLocation(){return alphaBlockVertsLocation;}//Move these to apporiate texture files
    static GLint  getAlphaBlockTextCoordsLocation(){return alphaBlockTextCoordsLocation;}
    static GLint  getAlphaBlockLocation(){return alphaBlockLocation;}




private:
    static GLuint baseProg;
    static GLuint rectProg;
    static GLuint texProg;
    static GLuint alphaBlock;


    static GLint textureVertsLocation;
    static GLint textureCoordsLocation;
    static GLint rectVertsLocation;
    static GLint rectColorLocation;

    static GLint alphaBlockVertsLocation;
    static GLint alphaBlockTextCoordsLocation;
    static GLint alphaBlockLocation;








};




#endif //KALASOFT_SHADER_H
