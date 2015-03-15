#version 400 core

layout(location = 0) in vec3 positionIn;
layout(location = 1) in vec3 texcoordIn;
layout(location = 2) in vec3 normalIn;
layout(location = 3) in vec3 tangentIn;
layout(location = 4) in vec3 bitangentIn;


out vec3 vPosition;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out vec3 vTexcoord;


void main()
{
    vPosition  = positionIn.xyz;
    vNormal    = normalIn;
    vBitangent = bitangentIn;
    vTangent   = tangentIn;
    
    vTexcoord  = vec3(texcoordIn.st,0);
}
