#version 400 core
layout(triangles, equal_spacing, cw) in;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

in vec3 tcPosition[];
in vec3 tcNormal[];
in vec3 tcTexcoord[];
in vec3 tcTangent[];
in vec3 tcBitangent[];
in vec3 tcSmoothedNormal[];

out vec3 tePosition;
out vec3 teTexcoord;
out vec3 teNormal;
out vec3 teTangent;
out vec3 teBitangent;
out vec3 teSmoothedNormal;

vec3 average(vec3 c,vec3 b,vec3 a){
	return gl_TessCoord.x * a + gl_TessCoord.y * b + gl_TessCoord.z * c;
}

void main()
{
		
    tePosition  = average(tcPosition[0],tcPosition[1],tcPosition[2]);
    teTexcoord  = average(tcTexcoord[0],tcTexcoord[1],tcTexcoord[2]);
    teNormal    = normalize(average(tcNormal[0],tcNormal[1],tcNormal[2]));
    teTangent   = normalize(average(tcTangent[0],tcTangent[1],tcTangent[2]));
    teBitangent = normalize(average(tcBitangent[0],tcBitangent[1],tcBitangent[2]));
    teSmoothedNormal = normalize(average(tcSmoothedNormal[0],tcSmoothedNormal[1],tcSmoothedNormal[2]));

    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(tePosition, 1);


}
