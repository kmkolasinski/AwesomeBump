#version 330 core
// Atrybuty wejsciowe
layout(location = 0) in vec3 positionIn;
layout(location = 1) in vec3 texcoordIn;
layout(location = 2) in vec3 normalIn;
layout(location = 3) in vec3 tangentIn;
layout(location = 4) in vec3 bitangentIn;


out vec3 texcoord;
out vec3 pos; 
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;


void main() {


	vec4 eyeTemp    =  ModelViewMatrix * vec4(positionIn, 1);
	gl_Position 		= ProjectionMatrix * eyeTemp;
	pos = (ModelMatrix*vec4(positionIn,0)).xyz;
	// Tekstury 3D zawieraja trzy wpsolrzedne - kierunek wektora bedzie wskazywal odpowiedni teksel tekstury 3D
	texcoord 			= 	(ModelMatrix*vec4(positionIn,0)).xyz;

}
