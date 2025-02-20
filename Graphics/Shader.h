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

//TODO use AssetManager to obtain Source;
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

    static GLuint createComputeProgram(const char *shaderPath);

public:

    static bool prepareShaders(AssetManager *assetManager);
    static GLuint getTextureProgram(){return texProg;}
    static GLuint getRectProgram(){return rectProg;}
    static GLuint getAlphaBlockProgram(){return alphaBlock;}
    static GLuint getMVPTextureProgram(){return mvpTextureProgram;}


    static GLint  getTextureVertsLocation(){return textureVertsLocation;}//Move these to apporiate texture files
    static GLint  getTextureCoordsLocation(){return textureCoordsLocation;}
    static GLint  getRectVertsLocation(){return rectVertsLocation;}
    static GLint  getRectColorLocation(){return rectColorLocation;}
    static GLint  getAlphaBlockVertsLocation(){return alphaBlockVertsLocation;}//Move these to apporiate texture files
    static GLint  getAlphaBlockTextCoordsLocation(){return alphaBlockTextCoordsLocation;}
    static GLint  getAlphaBlockLocation(){return alphaBlockLocation;}
    static GLint  getMVPVertLocation(){return mvpTextureVertsLocation;}
    static GLint  getMVPTextCoordsLocation(){return mvpTextureCoordLocation;}
    static GLint  getMVPModelLocation(){return mvpModelLocation;}
    static GLint  getMVPProjectionLocation(){return mvpProjectionLocation;}
    static GLint  getMVPViewLocation(){return mvpViewLocation;}






private:
    static GLuint baseProg;
    static GLuint rectProg;
    static GLuint texProg;
    static GLuint alphaBlock;
    static GLuint mvpTextureProgram;


    static GLint textureVertsLocation;
    static GLint textureCoordsLocation;
    static GLint rectVertsLocation;
    static GLint rectColorLocation;

    static GLint mvpTextureVertsLocation;
    static GLint mvpTextureCoordLocation;
    static GLint mvpModelLocation;
    static GLint mvpProjectionLocation;
    static GLint mvpViewLocation;

    static GLint alphaBlockVertsLocation;
    static GLint alphaBlockTextCoordsLocation;
    static GLint alphaBlockLocation;








};




#endif //KALASOFT_SHADER_H
