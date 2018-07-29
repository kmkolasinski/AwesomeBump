#version 330 core

layout(location = 0) in vec3 positionIn;
layout(location = 1) in vec3 texcoordIn;
layout(location = 2) in vec3 normalIn;
layout(location = 3) in vec3 tangentIn;
layout(location = 4) in vec3 bitangentIn;
layout(location = 5) in vec3 smoothedNormalIn;


uniform  float gui_uvScale;
uniform  vec2  gui_uvScaleOffset;


// Sending vertex attributes to the TC shader
out vec3 vPosition;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out vec3 vTexcoord;
out vec3 vSmoothedNormal;




void main()
{
    vPosition  = positionIn.xyz;
    vNormal    = normalIn;
    vBitangent = bitangentIn;
    vTangent   = tangentIn;
    vSmoothedNormal = smoothedNormalIn;
    vTexcoord  = vec3(texcoordIn.st,0)*gui_uvScale + vec3(gui_uvScaleOffset,0);
}
