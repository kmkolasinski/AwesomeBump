#version 330 core
layout(location = 1) in vec3 texcoordIn;
out vec3 vTexcoord;
void main()
{    
    vTexcoord  = vec3(texcoordIn.st,0);
}
