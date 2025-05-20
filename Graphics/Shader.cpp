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
    GLuint shader = glCreateShader(shaderType);

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
                KSLOGD("Shader : ","Vertex \n : %s",v);
                KSLOGD("Shader :", "Fragment \n %s",f);

            }
            else
            {
                KSLOGE("Shader :","read to buf error");
                return 0;
            }

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


//TODO move the above to approriate,Renderer,View,Construtors;(instance Count,SingleTons)
GLuint Shader::baseProg = 0;
GLuint Shader::rectProg = 0;
GLuint Shader::texProg = 0;
GLuint Shader::alphaBlock = 0;
GLuint Shader::mvpTextureProgram = 0;
GLuint Shader::blendProgram = 0;

//all should default to -1;
GLint  Shader::textureVertsLocation = 0;
GLint  Shader::textureCoordsLocation = 0;
GLint  Shader::rectVertsLocation = 0;
GLint  Shader::rectColorLocation = 0;
GLint  Shader::alphaBlockVertsLocation = 0;
GLint  Shader::alphaBlockTextCoordsLocation = 0;
GLint  Shader::alphaBlockLocation = 0;


GLint Shader::mvpTextureVertsLocation = 0;
GLint Shader::mvpTextureCoordLocation = 0;
GLint Shader::mvpModelLocation = 0;
GLint Shader::mvpProjectionLocation = 0;
GLint Shader::mvpViewLocation = 0;



/**
 *TODO handle failures aptly.
 *TODO add  locations inside the shaders manually.
 * ShaderManager instance for Appication,with shader in it;
 */
bool Shader::prepareShaders(AssetManager *assetManager)
{
    //std::string shaderPath = "shaders/pianoShaders/";//TODO close resources
    if(assetManager != nullptr)//TODO directly use assetManager SingleTon
    {
        IKSStream *vertA = AssetManager::openAsset("shaders/pianoShaders/texture.vert");
        IKSStream *fragA = AssetManager::openAsset("shaders/pianoShaders/texture.frag");
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

        vertA = assetManager->openAsset("shaders/pianoShaders/mvptexture.vert");
        fragA = assetManager->openAsset("shaders/pianoShaders/mvptexture.frag");
        assert(vertA && fragA);
        mvpTextureProgram = Shader::createProgram(vertA,fragA);
        if(mvpTextureProgram == 0)
        {
            KSLOGE("prepareShaders", "error compiling mvpTexture shader");
            return false;
        }

        mvpTextureVertsLocation = glGetAttribLocation(mvpTextureProgram,"verts");
        mvpTextureCoordLocation = glGetAttribLocation(mvpTextureProgram,"textureCoords");
        mvpModelLocation = glGetUniformLocation(mvpTextureProgram, "model");
        mvpProjectionLocation = glGetUniformLocation(mvpTextureProgram, "proj");
        mvpViewLocation = glGetUniformLocation(mvpTextureProgram, "view");

        vertA = assetManager->openAsset("common/shaders/texture.vert");
        fragA = assetManager->openAsset("common/shaders/addtextures.frag");
        assert(vertA && fragA);
        blendProgram = Shader::createProgram(vertA,fragA);

    }
    return true;
}


void Shader::clearShaders()
{
    glDeleteProgram(baseProg);
    glDeleteProgram(rectProg);
    glDeleteProgram(texProg);
    glDeleteProgram(alphaBlock);
    glDeleteProgram(mvpTextureProgram);
    glDeleteProgram(blendProgram);

}

GLuint Shader::createComputeProgram(const char *shaderPath)
{
    IKSStream *comStre = AssetManager::openAsset(shaderPath);
    if(!comStre)return 0;
    long fSize = comStre->getSize();

    char *comSrc = static_cast<char *>(malloc(fSize + 1));
    GLuint  computeShader = 0;
    if(comSrc)
    {
        int sRead = 0;
        sRead = comStre->read(comSrc,fSize);

        if(sRead == fSize)
        {
            comSrc[fSize] = '\0';
            KSLOGD("Compute Shader :", "Fragment \n %s",comSrc);


        }
        else
        {
            KSLOGE("Shader :","read to buf error");
            free(comSrc);
            return 0;
        }

        computeShader = compile(GL_COMPUTE_SHADER,comSrc);
        free(comSrc);
    }

    GLuint program= glCreateProgram();
    if(computeShader && program)
    {

        glAttachShader(program,computeShader);
        glLinkProgram(program);
        GLint linkStatus=GL_FALSE;
        glGetProgramiv(program,GL_LINK_STATUS,&linkStatus);
        if(linkStatus!=GL_TRUE)
        {
            GLint buflen=0;
            glGetProgramiv(program,GL_INFO_LOG_LENGTH,&buflen);
            if(buflen)
            {
                char* buf= (char *)malloc(buflen);
                if(buf)
                {
                    glGetProgramInfoLog(program,buflen,NULL,buf);
                    KSLOGE(LOGTAG,"could not link the  Compute program reason : %s",buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program=0;
        }
        else
        {
            KSLOGD(LOGTAG,"ComputeShader Linked Succesfully");
            return program;

        }
    }
    else
    {
        KSLOGE(LOGTAG,"Couldnot create compute progam");
    }
    return program;
}

//TODO reduce the function/reuse with others;//get source from file directly for each file
GLuint Shader::createTransformFeedbackProgram(const char *vertexPath, const char *fragmentPath,const char **varyings,int varyingCount)
{

    GLuint program = 0;
    IKSStream *vert = AssetManager::openAsset(vertexPath);
    IKSStream *frag = AssetManager::openAsset(fragmentPath);
    if(vert && frag)
    {
        char *v = nullptr,*f = nullptr;
        long vSize = vert->getSize() ;
        long fSize = frag->getSize();

        v = static_cast<char *>(malloc(vSize + 1));
        f = static_cast<char *>(malloc(fSize + 1));

        if(v && f)
        {
            int vRead = 0,fRead = 0;
            vRead = vert->read(v,vSize);
            fRead =frag->read(f,fSize);

            if(vRead == vSize && fRead == fSize)
            {
                v[vSize] = '\0';
                f[fSize] = '\0';
                KSLOGD("Shader : ","Vertex \n : %s",v);
                KSLOGD("Shader :", "Fragment \n %s",f);

            }
            else
            {
                KSLOGE("Shader :","read to buf error");
                return 0;
            }
            GLuint vs = compile(GL_VERTEX_SHADER,v);
            GLuint fs = compile(GL_FRAGMENT_SHADER,f);

            program=glCreateProgram();
            glAttachShader(program,vs);
            glAttachShader(program,fs);
            if(varyingCount > 0)
            glTransformFeedbackVaryings(program,varyingCount,varyings,GL_INTERLEAVED_ATTRIBS);
            glLinkProgram(program);

        }


        if(v)
            free(v);
        if(f)
            free(f);
    }else
    {
        KSLOGE(LOGTAG,"FeedbackProgram create error");
    }
    return program;
}
