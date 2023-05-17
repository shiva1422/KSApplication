
const char* vertShader= GLSLTOSTRING(
\#version 310 es \n
precision highp float;
layout(location=0) in vec2 verts;
layout(location=1) in vec4 color;
layout(location=2) in vec2 texCoords;
layout(location=0) uniform vec2 scale;
layout(location=2) uniform vec2 translate;
out vec2 uv;
out vec2 textCoodsOut;


void main()
{

    gl_Position = vec4(verts,0.0 ,1.0);
    textCoodsOut = texCoords;
}
);


const char* fragShader= GLSLTOSTRING(
\#version 310 es \n
precision highp float;
in vec2 textCoodsOut;
out vec2 uv;
out vec4 Fragcolor;
uniform sampler2D image;

void main()
{
    vec2 finalTexCoods=vec2(textCoodsOut.x,1.0-textCoodsOut.y);//cause tex cood start at botoom whare as image from top

   Fragcolor = texture(image,finalTexCoods);
}
);