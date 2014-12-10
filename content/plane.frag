uniform sampler2D texDiffuse;
uniform sampler2D texNormal;
uniform sampler2D texSpecular;
uniform sampler2D texHeight;
uniform sampler2D texSSAO;
uniform bool gui_bSpecular;
uniform bool gui_bOcclusion;
uniform float gui_SpecularIntensity;
uniform float gui_DiffuseIntensity;

varying vec4 texc;
varying vec3 vertexNormal;  
varying vec4 vertexPosition; 
varying vec4 lightPosition; 
varying vec4 staticLightPosition; 
void main(void)
{
    vec4 diffuseColor       =  texture2D(texDiffuse , texc.st);
    vec4  normalColor      =  texture2D(texNormal  , texc.st);
    vec4 specularColor     =  texture2D(texSpecular, texc.st);
	float ambientOclusion =  gui_bOcclusion*texture2D(texSSAO, texc.st).r+!gui_bOcclusion;
	
	
	// Obliczenia zwiazane z modelem phonga
	vec3 N = normalize(vertexNormal);
	vec3 L = normalize(lightPosition-vertexPosition);
	vec3 V = normalize(vertexPosition);
		
	
	vec3 L2 = normalize(staticLightPosition-vertexPosition);
	
	// Oblicza wpolczynnik swiatla rozproszonego 
	float Rd = 1.8*max(0.0, dot(L, N));	
	float Rd2 = 0.2*max(0.0, dot(L2, N));
	//	Kolor zwiazany z swiatlem rozproszonym
	vec3 diffuse = gui_DiffuseIntensity * (Rd+Rd2) * diffuseColor.rgb / 2.0;

	// Obliczenia zwiazane ze swiatlem odbitym
	vec3 R  = reflect(L, N);
	vec3 R2  = reflect(L2, N);
	float Rs   = 15*pow(max(0.0, dot(V, R)), 20);
	float Rs2 = 5*pow(max(0.0, dot(V, R2)), 20);
	vec3 Ts = specularColor.rgb;
	
	vec3 specular = gui_SpecularIntensity * (Rs+Rs2) * Ts/2 * gui_bSpecular ;

	gl_FragColor  = vec4(diffuse*ambientOclusion + specular*ambientOclusion, 1);

	
};
