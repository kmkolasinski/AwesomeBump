#version 330

layout(location = 0) in vec4 positionIn;
layout(location = 1) in vec2 texCoord;
out vec2 fragTexCoord;

void main( void )
{
    gl_Position = positionIn;
    fragTexCoord = texCoord;
}
