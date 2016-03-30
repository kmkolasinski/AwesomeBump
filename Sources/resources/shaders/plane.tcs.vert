#version 400 core
layout(vertices = 3) out;

#define ID gl_InvocationID

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform int gui_shading_type;
uniform  float gui_depthScale;
uniform bool gui_bHeight;
uniform bool gui_bUseCullFace;
uniform int gui_noTessSub;
in vec3 vPosition[];
in vec3 vNormal[];
in vec3 vTexcoord[];
in vec3 vTangent[];
in vec3 vBitangent[];
in vec3 vSmoothedNormal[];

out vec3 tcPosition[];
out vec3 tcNormal[];
out vec3 tcTexcoord[];
out vec3 tcTangent[];
out vec3 tcBitangent[];
out vec3 tcSmoothedNormal[];


float lod_factor = 0.004;
float level(vec4 v0, vec4 v1){     
         return clamp(distance(v0.xyz, v1.xyz)/lod_factor, 1, gui_noTessSub);
 }

vec4 project(vec3 vertex){
    vec4 result =    ModelViewMatrix * vec4(vertex,1);
    //result /= result.w;
    return result;
}

void main()
{
    tcPosition[ID] 	= vPosition[ID];
    tcNormal[ID]   	= vNormal[ID];
    tcTangent[ID]       = vTangent[ID];
    tcBitangent[ID]     = vBitangent[ID];
    tcTexcoord[ID] 	= vTexcoord[ID];
    tcSmoothedNormal[ID]= vSmoothedNormal[ID];


    if (ID == 0) {		
    if(gui_shading_type != 2 || !gui_bHeight){
            gl_TessLevelInner[0] = 1;
            gl_TessLevelOuter[0] = 1;
            gl_TessLevelOuter[1] = 1;
            gl_TessLevelOuter[2] = 1;
    }else{
            vec4 ss0 = project(vPosition[0]);
            vec4 ss1 = project(vPosition[1]);
            vec4 ss2 = project(vPosition[2]);
            float e0 = level(ss1, ss2);
            float e1 = level(ss0, ss2);
            float e2 = level(ss1, ss0);

            vec3 normal = NormalMatrix    * cross( vPosition[2] - vPosition[0] , vPosition[1] - vPosition[0]);
            vec4 eyePos = ModelViewMatrix * vec4(vPosition[0],1);
            float dtv = dot(normalize(normal),normalize(eyePos.xyz));

            if( dtv < -0.0 && gui_bUseCullFace ){
                    e0 = 0;
                    e1 = 0;
                    e2 = 0;
            }


            gl_TessLevelInner[0] = (e0+e1+e2)/3;
            gl_TessLevelOuter[0] = e0;
            gl_TessLevelOuter[1] = e1;
            gl_TessLevelOuter[2] = e2;
    }

    }
	
}
