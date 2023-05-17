//
// Created by kalasoft on 10/9/21.
//

#include <Logger/KSLog.h>
#include <cstdlib>
#include "Shader.h"

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
                       KSLog::error("could not link the  program reason : %s",buf);
                       free(buf);
                   }
               }
               glDeleteProgram(program);
               program=0;
           }
           else
               KSLog::debug("linkShader","success");
       }
   }
   else
       KSLog::error(LOGTAG,"vert of frag shader not compiled");

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

           // KSLog::error(LOGTAG,"shader not compiled getting failure info .....");
            GLint  infolen=0;
            glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infolen);
            if(infolen)
            {
                char* buf=(char *)malloc(infolen);
                if(buf)
                {
                    glGetShaderInfoLog(shader,infolen,NULL,buf);
                    KSLog::error(LOGTAG,"compile failed : \n reason %s",buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
        else KSLog::debug(LOGTAG,"compile  Success");
    }
    else
    KSLog::error(LOGTAG,"compile error %s",shaderType==GL_VERTEX_SHADER?"vertex" : "fragment");//TODO shaderType to string this is not accurate;
    return shader;

}
GLuint Shader::createProgram(const char *vertSrc, const char *fragSrc)
{
    GLuint v = compile(GL_VERTEX_SHADER,vertSrc);
    GLuint f = compile(GL_FRAGMENT_SHADER,fragSrc);
    /**TODO
     * Decide to keep or delete v,f or v can be same for image processing
     */
     KSLog::debug(LOGTAG,"VID %i FID %i",v,f);
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
GLuint Shader::texProg = 0;
