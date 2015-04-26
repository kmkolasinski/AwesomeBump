#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 36) out;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;



out vec3 WSNormal;


int findices[36] = int[](0, 1, 2, 2,3,0,
                         4, 5, 6, 6,7,4,
                         4, 0, 1, 1,5,4,
                         2, 6, 7, 7,3,2,
                         1, 5, 6, 6,2,1,
                         0, 4, 7, 7,3,0);
						 
int sindices[6] = int[](0,1,2,2,3,0);
int layers[6]   = int[](4,5,2,3,1,0);




void main()
{
  
  vec3 positions[8];
  positions[0] = vec3(-1.0,-1.0,-1.0 );
  positions[1] = vec3( 1.0,-1.0,-1.0 );
  positions[2] = vec3( 1.0,+1.0,-1.0 );
  positions[3] = vec3(-1.0,+1.0,-1.0 );
  positions[4] = vec3(-1.0,-1.0,+1.0 );
  positions[5] = vec3( 1.0,-1.0,+1.0 );
  positions[6] = vec3( 1.0,+1.0,+1.0 );
  positions[7] = vec3(-1.0,+1.0,+1.0 );
  
  vec3 screen_positions[4];
  screen_positions[0] = vec3(-1.0,-1.0,0.0);
  screen_positions[1] = vec3( 1.0,-1.0,0.0);
  screen_positions[2] = vec3( 1.0,+1.0,0.0);
  screen_positions[3] = vec3(-1.0,+1.0,0.0);
  
  
  for(int f = 0 ; f < 6 ; f++){
	gl_Layer = layers[f]; 
	for(int t = 0; t < 2 ; t++){
		for(int v = 0; v < 3 ; v++){
			int id = 6*f + 3*t + v;
			int sid = 3*t + v;

			WSNormal    =-normalize(positions[findices[id]]);
			// lazy empirical rotations corrections
                        if(f==0){
                            WSNormal = normalize(vec3(-WSNormal.x,WSNormal.y, WSNormal.z)); // fixing rotations
                        }
                        if(f==2){
                            WSNormal = normalize(vec3( WSNormal.z,WSNormal.y,-WSNormal.x)); // fixing rotations
                        }
                        if(f==3){
                            WSNormal  = normalize(vec3(-WSNormal.z,WSNormal.y,-WSNormal.x)); // fixing rotations
                        }
                        if(f==4){
                            WSNormal = normalize(vec3( WSNormal.x,WSNormal.y,-WSNormal.z)); // fixing rotations
                         }

                        vec2 pos    = sign(screen_positions[sindices[sid]]).st;
                        gl_Position = vec4(pos,0,1);
			EmitVertex();
		}//vertex
		EndPrimitive(); 
	}// triangle
		
  } // face
  
}
