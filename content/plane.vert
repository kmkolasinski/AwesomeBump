#version 400 core

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 texCoord;
uniform sampler2D texHeight;
uniform sampler2D texNormal;

uniform  mat4  ModelViewMatrix;
uniform  mat4  ProjectionMatrix;
uniform  mat3  NormalMatrix;
uniform  vec4  lightPos;
uniform  float gui_depthScale;
uniform  float gui_uvScale;
uniform  vec2  gui_uvScaleOffset;

out vec4 texc;
out vec3 vertexNormal;
out vec4 vertexPosition;
out vec4 lightPosition;
out vec4 staticLightPosition;

void main(void)
{
    texc = (texCoord)*gui_uvScale+vec4(gui_uvScaleOffset,0,0);

    float hPos  =  texture(texHeight , texc.st ).x;

    vertexPosition = ModelViewMatrix * ( vertex + vec4(0,0,hPos*0.1*gui_depthScale,0));
    lightPosition    = lightPos;// ModelViewMatrix *  lightPos;
    staticLightPosition = ModelViewMatrix *  vec4(0,0,5.0,1);
    vertexNormal  = NormalMatrix * normalize(texture(texNormal , texc.st).rgb - 0.5);
    gl_Position = ProjectionMatrix * (vertexPosition);
};
