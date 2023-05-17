//
// Created by kalasoft on 10/9/21.
//

#ifndef KALASOFT_SHADER_H
#define KALASOFT_SHADER_H

#include <KSIO/IKSStream.h>
#include "GLContext.h"


/*
 * Context should be current on this thread to create ShaderProgram.
 */


class Shader{


public:
    //reads the stream ,creates program and return.also Streams are closed here;
    static GLuint createProgram(IKSStream *vertStr, IKSStream *fragStr);

    static GLuint createProgram(const char *vertSrc,const char *fragSrc);


public:
    static GLuint baseProg;
    static GLuint texProg;

};




#endif //KALASOFT_SHADER_H
