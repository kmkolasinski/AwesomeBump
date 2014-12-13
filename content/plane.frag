#version 400 core
uniform sampler2D texDiffuse;
uniform sampler2D texNormal;
uniform sampler2D texSpecular;
uniform sampler2D texHeight;
uniform sampler2D texSSAO;
uniform bool gui_bSpecular;
uniform bool gui_bOcclusion;
uniform float gui_SpecularIntensity;
uniform float gui_DiffuseIntensity;

in vec4 texc;
in vec3 vertexNormal;
in vec4 vertexPosition;
in vec4 lightPosition;
in vec4 staticLightPosition;
out vec4 FragColor;
void main(void)
{
    vec4  diffuseColor     =  texture(texDiffuse , texc.st);
    vec4  normalColor      =  texture(texNormal  , texc.st);
    vec4  specularColor    =  texture(texSpecular, texc.st);
    float ambientOclusion  =  float(gui_bOcclusion)*texture(texSSAO, texc.st).r+float(!gui_bOcclusion);
	
	
    // Phong model
    vec3 N = normalize(vertexNormal.xyz);
    vec3 L = normalize(lightPosition.xyz-vertexPosition.xyz);
    vec3 V = normalize(vertexPosition.xyz);


    vec3 L2 = normalize(staticLightPosition.xyz-vertexPosition.xyz);

    // diffuse lightning
    float Rd     = 1.8*max(0.0, dot(L, N));
    float Rd2    = 0.2*max(0.0, dot(L2, N));
    vec3 diffuse = gui_DiffuseIntensity * (Rd+Rd2) * diffuseColor.rgb / 2.0;

    // specular lightning
    vec3 R    = reflect(L, N);
    vec3 R2   = reflect(L2, N);
    float Rs  = 15*pow(max(0.0, dot(V, R)), 20);
    float Rs2 = 5*pow(max(0.0, dot(V, R2)), 20);
    vec3 Ts   = specularColor.rgb;

    vec3 specular = gui_SpecularIntensity * (Rs+Rs2) * Ts/40 * float(gui_bSpecular) ;
    FragColor     = vec4(diffuse*ambientOclusion + specular*ambientOclusion, 1);
};
