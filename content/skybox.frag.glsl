#version 400  core

uniform samplerCube texEnv;


in vec3 texcoord;
in vec3 pos;
out vec4 FragColor;

void main( void )
{
   // Tutaj tylko rysujemy teskture 3D   
   FragColor = texture(texEnv, texcoord);
   //FragColor = vec4(texcoord,1);
   //FragColor = texture(texEnv, normalize(pos));
  // FragColor = vec4(1);
}