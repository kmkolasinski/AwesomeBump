#version 330 core

layout(location = 0) in vec3 positionIn;

uniform vec2 quad_scale; // skala od (0,0) do (1,1)
uniform vec2 quad_pos;   // przesuniecie  od (0,0) do (1,1)



out vec2 v2QuadCoords;

void main() {

    vec2 pos      = sign(positionIn.xy);
    v2QuadCoords  = (pos + 1)*0.5 ;
    if( quad_scale.x == 0 && quad_scale.y == 0 ){
            gl_Position      = vec4(pos,0,1);
    }else
            gl_Position      = vec4(pos*quad_scale-(vec2(1)-quad_scale)+quad_pos*2,0,1);

	
}
