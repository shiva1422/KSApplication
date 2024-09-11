//
// Created by shivaaz on 8/28/24.
//

#include "RectView.h"
#include "Shader.h"
void RectView::draw() {

   // GLView::draw();
   //color can actually embedded in vertices;
    glEnable(GL_BLEND);//TODO global management;
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   //TODO vao
    glUseProgram(Shader::getRectProgram());
    glEnableVertexAttribArray(Shader::getRectVertsLocation());
    glVertexAttribPointer(Shader::getRectVertsLocation(),2,GL_FLOAT,GL_FALSE,0,(void *)getVertices());

    glUniform4f(Shader::getRectColorLocation(),r,g,b,a);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,GLView::defaultIndexBufId);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT, nullptr);

    GLContext::getError("RectView");
    KSLOGD("RectView  : ", " %d %d %d",Shader::getRectProgram(),Shader::getRectVertsLocation(),Shader::getRectColorLocation());
    glDisableVertexAttribArray(Shader::getRectVertsLocation());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    glDisable(GL_BLEND);

}
