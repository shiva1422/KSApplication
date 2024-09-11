//
// Created by kalasoft on 10/9/21.
//

#include <Logger/KSLog.h>
#include <cstdlib>
#include <assert.h>
#include "Shader.h"
#include <KSIO/AssetManager.h>

#define LOGTAG "GLShader"

//TODO delete intermediates
GLuint linkShaders(GLuint vertexShaderId, GLuint fragmentShaderId)
{
    GLuint program = 0;
   if(vertexShaderId && fragmentShaderId)
   {
       program=glCreateProgram();
       if(program)
       {
           glAttachShader(program,vertexShaderId);
           glAttachShader(program,fragmentShaderId);
           glLinkProgram(program);
           GLint linkStatus=GL_FALSE;
           glGetProgramiv(program,GL_LINK_STATUS,&linkStatus);
           if(linkStatus != GL_TRUE)
           {
               GLint buflen=0;
               glGetProgramiv(program,GL_INFO_LOG_LENGTH,&buflen);
               if(buflen)
               {
                   char* buf=(char *)malloc(buflen);
                   if(buf)
                   {
                       glGetProgramInfoLog(program,buflen,NULL,buf);
                      KSLOGE(LOGTAG,"could not link the  program reason : %s",buf);
                       free(buf);
                   }
               }
               glDeleteProgram(program);
               program=0;
           }
           else
               KSLOGD("linkShader","success");
       }
   }
   else
      KSLOGE(LOGTAG,"vert of frag shader not compiled");

    return program;
}

GLuint compile(GLenum shaderType,const char *source)
{
    GLuint shader=glCreateShader(shaderType);

    if(shader)
    {

        glShaderSource(shader,1,&source,NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
        if(!compiled)
        {

           //KSLOGE(LOGTAG,"shader not compiled getting failure info .....");
            GLint  infolen=0;
            glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infolen);
            if(infolen)
            {
                char* buf=(char *)malloc(infolen);
                if(buf)
                {
                    glGetShaderInfoLog(shader,infolen,NULL,buf);
                   KSLOGE(LOGTAG,"compile failed : \n reason %s",buf);
                   KSLOGD(LOGTAG,"\n Shader %s",source);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
        else KSLOGD(LOGTAG,"compile  Success");
    }
    else
   KSLOGE(LOGTAG,"compile error %s",shaderType==GL_VERTEX_SHADER?"vertex" : "fragment");//TODO shaderType to string this is not accurate;
    return shader;

}
GLuint Shader::createProgram(const char *vertSrc, const char *fragSrc)
{
    GLuint v = compile(GL_VERTEX_SHADER,vertSrc);
    GLuint f = compile(GL_FRAGMENT_SHADER,fragSrc);
    /**TODO
     * Decide to keep or delete v,f or v can be same for image processing
     */
     KSLOGD(LOGTAG,"VID %i FID %i",v,f);
    return linkShaders(v,f);
}

GLuint Shader::createProgram(IKSStream *vertStr, IKSStream *fragStr)
{

    GLuint program = 0;

    if(vertStr && fragStr)
    {
        /*TODO
       * Doing TOO much work ,do read out of this
        */

        char *v = nullptr,*f = nullptr;
        long vSize = vertStr->getSize() ;
        long fSize = fragStr->getSize();

        v = static_cast<char *>(malloc(vSize + 1));
        f = static_cast<char *>(malloc(fSize + 1));

        if(v && f)
        {
            int vRead = 0,fRead = 0;
            vRead = vertStr->read(v,vSize);
            fRead =fragStr->read(f,fSize);

            if(vRead == vSize && fRead == fSize)
            {
                v[vSize] = '\0';
                f[fSize] = '\0';

            }
            //else TODO

           program =  createProgram(v,f);
        }

        if(v)
            free(v);
        if(f)
            free(f);

    }

    //TODO delete called on abstract but no virtual destructor
    delete vertStr;
    delete fragStr;

    return program;

}


GLuint Shader::baseProg = 0;
GLuint Shader::rectProg = 0;
GLuint Shader::texProg = 0;
GLuint Shader::alphaBlock;

GLint  Shader::textureVertsLocation = 0;
GLint  Shader::textureCoordsLocation = 0;
GLint  Shader::rectVertsLocation = 0;
GLint  Shader::rectColorLocation = 0;
GLint  Shader::alphaBlockVertsLocation = 0;
GLint  Shader::alphaBlockTextCoordsLocation = 0;
GLint Shader::alphaBlockLocation = 0;

/**
 *TODO handle failures aptly.
 *TODO add  locations inside the shaders manually.
 */
bool Shader::prepareShaders(AssetManager *assetManager)
{
    if(assetManager != nullptr)
    {
        IKSStream *vertA = assetManager->openAsset("shaders/pianoShaders/texture.vert");
        IKSStream *fragA = assetManager->openAsset("shaders/pianoShaders/texture.frag");
        assert(vertA && fragA);
        texProg = Shader::createProgram(vertA,fragA);
        if(texProg == 0)
        {
            KSLOGE("prepareShaders", "error compiling texture shader");
            return false;
        }
        textureVertsLocation  = glGetAttribLocation(texProg, "verts");
        textureCoordsLocation  = glGetAttribLocation(texProg,"textureCoords");

        vertA = assetManager->openAsset("shaders/pianoShaders/rect.vert");
        fragA = assetManager->openAsset("shaders/pianoShaders/rect.frag");
        assert(vertA && fragA);
        rectProg = Shader::createProgram(vertA,fragA);
        if(rectProg == 0)
        {
            KSLOGE("prepareShaders", "error compiling rect shader");
            return false;
        }
        rectVertsLocation  = glGetAttribLocation(rectProg, "verts");
        rectColorLocation  = glGetUniformLocation(rectProg,"color");

        vertA = assetManager->openAsset("shaders/pianoShaders/alphablock.vert");
        fragA = assetManager->openAsset("shaders/pianoShaders/alphablock.frag");
        assert(vertA && fragA);
        alphaBlock = Shader::createProgram(vertA,fragA);
        if(alphaBlock == 0)
        {
            KSLOGE("prepareShaders", "error compiling alphablock shader");
            return false;
        }
        alphaBlockVertsLocation  = glGetAttribLocation(alphaBlock, "verts");
        alphaBlockTextCoordsLocation  = glGetAttribLocation(alphaBlock,"textureCoords");
        alphaBlockLocation = glGetUniformLocation(alphaBlock,"blockParams");

    }
    return true;
}
