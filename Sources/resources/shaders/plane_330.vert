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
out vec3 tePosition;
out vec3 teNormal;
out vec3 teTangent;
out vec3 teBitangent;
out vec3 teTexcoord;
out vec3 teSmoothedNormal;




void main()
{
    tePosition  = positionIn.xyz;
    teNormal    = normalIn;
    teBitangent = bitangentIn;
    teTangent   = tangentIn;
    teSmoothedNormal = smoothedNormalIn;
    teTexcoord  = vec3(texcoordIn.st,0)*gui_uvScale + vec3(gui_uvScaleOffset,0);
}
