#version 400 core

// Uniform variables
uniform samplerCube texEnv;


// output color
out vec4 FragColor;

// input variables
in vec3 WSTangent;
in vec3 WSBitangent;
in vec3 WSNormal;


const float PI  = 3.14159;
const float PI2 = PI/2;


vec4 PBR_Diffuse(vec3 surfaceColor,samplerCube texDiffuseEnv,
                           vec3 surfaceNormal,
                           vec3 surfaceTangent,
                           vec3 surfaceBitangent){

    vec3 ocolor = vec3(0);
    //ocolor = texture(texDiffuseEnv, surfaceNormal).rgb;
    //return vec4(ocolor,1) ;
	
    int index = 0;
    for(float phi = 0; phi < 2 * PI ; phi += 0.1)
        {
            for(float theta = 0; theta < PI2 ; theta += 0.05)
            {
                vec3 temp = cos(phi) * surfaceTangent + sin(phi) * surfaceBitangent;
                vec3 sampleVector = cos(theta) * surfaceNormal + sin(theta) * temp;
                ocolor += texture( texDiffuseEnv, sampleVector ).rgb * cos(theta) * sin(theta);
                index ++;
            }
        }
    return vec4(surfaceColor*ocolor*PI/ index,1);
	
}

void main( void )
{


    FragColor = PBR_Diffuse(vec3(1),
                        texEnv,
                        normalize(WSNormal),
                        normalize(WSTangent),
                        normalize(WSBitangent));
							
}
