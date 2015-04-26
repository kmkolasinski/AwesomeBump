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


/*
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform  float meshScale;
uniform  float gui_depthScale;
uniform  float gui_uvScale;
uniform  vec4  cameraPos;
uniform  vec4  lightPos;
uniform bool gui_bHeight;
uniform int gui_shading_type;


out vec3 texcoord;
out vec3 ESVertexPosition;
out vec3 ESVertexNormal;
out vec3 ESVertexTangent;
out vec3 ESVertexBitangent;

const int no_lights = 2;
out vec3 TSLightPosition[no_lights];
out vec3 TSViewDirection[no_lights];
out vec3 TSHalfVector;

void bump_mapping(vec3 eyeLightDir[2],vec3 eyeVertexDir[2],vec3 halfVector){


   vec3 fvVertexNormal      = ESVertexNormal;
   vec3 fvBiVertexNormal    = ESVertexBitangent;
   vec3 fvTangent           = ESVertexTangent;

    // Tangent space calculation of two lights
   for(int lightIndeks = 0; lightIndeks < 2 ; lightIndeks++){
       TSViewDirection[lightIndeks].x  = dot( fvTangent          , eyeVertexDir[lightIndeks] );
       TSViewDirection[lightIndeks].y  = dot( fvBiVertexNormal   , eyeVertexDir[lightIndeks] );
       TSViewDirection[lightIndeks].z  = dot( fvVertexNormal     , eyeVertexDir[lightIndeks] );

       TSLightPosition[lightIndeks].x  = dot( fvTangent          , eyeLightDir[lightIndeks] );
       TSLightPosition[lightIndeks].y  = dot( fvBiVertexNormal   , eyeLightDir[lightIndeks] );
       TSLightPosition[lightIndeks].z  = dot( fvVertexNormal     , eyeLightDir[lightIndeks] );
   }

   TSHalfVector.x  = dot( fvTangent          , halfVector );
   TSHalfVector.y  = dot( fvBiVertexNormal   , halfVector );
   TSHalfVector.z  = dot( fvVertexNormal     , halfVector );

}



void main()
{


        texcoord                = texcoordIn;
        ESVertexNormal 	   	= NormalMatrix * normalIn;
        ESVertexTangent		= NormalMatrix * tangentIn;
        ESVertexBitangent       = NormalMatrix * bitangentIn;
        vec4 eyeVec 	   	= ModelViewMatrix * vec4(positionIn,1);
        ESVertexPosition   	= eyeVec.xyz;

        vec3 eyeLightVectors[2];
        eyeLightVectors[0] = normalize(cameraPos.xyz-eyeVec.xyz);
        eyeLightVectors[1] = normalize(lightPos.xyz-eyeVec.xyz);

        vec3 eyeViewVectors[2];
        eyeViewVectors[0] = -normalize(eyeVec.xyz);
        eyeViewVectors[1] = -normalize(eyeVec.xyz);

        vec4 lightV =  vec4(0,0,5,0);
        bump_mapping(eyeLightVectors,eyeViewVectors,normalize(lightV.xyz+eyeVec.xyz));
        gl_Position        	= ProjectionMatrix * eyeVec;


}
*/
