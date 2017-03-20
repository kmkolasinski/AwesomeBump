#version 330

in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main( void )
{
    fragColor =  texture(tex, fragTexCoord);
}
