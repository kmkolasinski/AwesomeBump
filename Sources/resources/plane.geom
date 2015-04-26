//#version 400 core
//layout(triangle_strip, max_vertices = 3) out;

layout(triangles) in;

uniform sampler2D texHeight;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform  float meshScale;
uniform  float gui_depthScale;
uniform  float gui_uvScale;
uniform  vec3  cameraPos;
uniform  vec4  lightPos;
uniform bool gui_bHeight;
uniform int gui_shading_type;

in vec3 tePosition[3];
in vec3 teTexcoord[3];
in vec3 teNormal[3];
in vec3 teTangent[3];
in vec3 teBitangent[3];
in vec3 teSmoothedNormal[3];


out vec3 texcoord;
out vec3 ESVertexPosition;
out vec3 ESVertexNormal;
out vec3 ESVertexTangent;
out vec3 ESVertexBitangent;
out vec3 ESHalfVector;

const int no_lights = 2;
out vec3 TSLightPosition[no_lights];
out vec3 TSViewDirection[no_lights];
out vec3 TSHalfVector;
out mat3 TBN;

out vec3 WSNormal;
out vec3 WSTangent;
out vec3 WSBitangent;
out vec3 WSPosition;


void bump_mapping(vec3 eyeLightDir[2],vec3 eyeVertexDir[2],vec3 halfVector){


   vec3 fvVertexNormal      = ESVertexNormal;
   vec3 fvBiVertexNormal    = ESVertexBitangent;
   vec3 fvTangent           = ESVertexTangent;

    // Tangent space calculation of two lights
   TBN = transpose(mat3(fvTangent,fvBiVertexNormal,fvVertexNormal));

   for(int lightIndeks = 0; lightIndeks < 2 ; lightIndeks++){
       TSViewDirection[lightIndeks] =  TBN * eyeVertexDir[lightIndeks];
       TSLightPosition[lightIndeks] =  TBN * eyeLightDir[lightIndeks];
   }
   TSHalfVector = TBN * halfVector;
   ESHalfVector = halfVector;

}


void main()
{

	vec3 newPos[3];
	float hfactor = 0;
	if(gui_bHeight && gui_shading_type == 2) hfactor = 1;
	for(int i = 0 ; i < 3 ; i++){

                float height = texture( texHeight, teTexcoord[i] .xy ).r;
                newPos[i] = tePosition[i] + height*normalize(teSmoothedNormal[i])*gui_depthScale/meshScale*0.05/gui_uvScale*hfactor;
	}

	
    for(int i = 0 ; i < 3 ; i++){
                texcoord.st     = (teTexcoord[i].st);
		
                ESVertexNormal 	   	= NormalMatrix * teNormal[i];
		ESVertexTangent		= NormalMatrix * teTangent[i];
                ESVertexBitangent       = NormalMatrix * teBitangent[i];
		vec4 eyeVec 	   	= ModelViewMatrix * vec4(newPos[i],1);
                vec4 eyeLight           = ModelViewMatrix * lightPos;

		ESVertexPosition   	= eyeVec.xyz;
		
		vec3 eyeLightVectors[2];
                eyeLightVectors[0] = normalize(-eyeVec.xyz);
                eyeLightVectors[1] = normalize(lightPos.xyz-eyeVec.xyz);

		vec3 eyeViewVectors[2];
		eyeViewVectors[0] = -normalize(eyeVec.xyz);
		eyeViewVectors[1] = -normalize(eyeVec.xyz);

                vec4 lightV =  vec4(0,0,5,0);
		bump_mapping(eyeLightVectors,eyeViewVectors,normalize(lightV.xyz+eyeVec.xyz));
		gl_Position        	= ProjectionMatrix * eyeVec; 

                mat3 mmat = mat3(ModelMatrix);
                WSNormal    = mmat*teNormal[i];
                WSTangent   = mmat*teTangent[i];
                WSBitangent = mmat*teBitangent[i];
                WSPosition  = (ModelMatrix * vec4( newPos[i],1)).xyz;

                TBN = transpose(mat3(WSTangent,WSBitangent,WSNormal));

		EmitVertex();
	}

	

    EndPrimitive(); 
		
}
