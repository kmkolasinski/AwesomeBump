attribute  vec4 vertex;
attribute  vec4 texCoord;

uniform  mat4 ModelViewMatrix;
uniform  mat4 ProjectionMatrix;
uniform  mat3 NormalMatrix;
uniform sampler2D texHeight;
uniform sampler2D texNormal;
uniform vec4 lightPos;
uniform float gui_depthScale;
uniform float gui_uvScale;
uniform vec2 gui_uvScaleOffset;

varying  vec4 texc;
varying vec3 vertexNormal;  
varying vec4 vertexPosition; 
varying vec4 lightPosition; 
varying vec4 staticLightPosition; 

void main(void)
{

    texc = (texCoord)*gui_uvScale+vec4(gui_uvScaleOffset,0,0);
    float hPos  =  texture2D(texHeight , texc.st ).r;
	
	
	vertexPosition = ModelViewMatrix * ( vertex + vec4(0,0,hPos*0.05*gui_depthScale,0));
	lightPosition    = lightPos;// ModelViewMatrix *  lightPos;
	staticLightPosition = ModelViewMatrix *  vec4(0,0,5.0,1);
	vertexNormal  = NormalMatrix * normalize(texture2D(texNormal , texc.st).rgb - 0.5);
    gl_Position = ProjectionMatrix * (vertexPosition);
};
