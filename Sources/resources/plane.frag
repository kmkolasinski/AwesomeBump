#version 400 core



// Uniform variables
uniform sampler2D texDiffuse;
uniform sampler2D texNormal;
uniform sampler2D texSpecular;
uniform sampler2D texHeight;
uniform sampler2D texSSAO;

uniform bool gui_bSpecular;
uniform bool gui_bOcclusion;
uniform bool gui_bHeight;
uniform bool gui_bDiffuse;
uniform bool gui_bNormal;
uniform  float gui_depthScale;
uniform float gui_SpecularIntensity;
uniform float gui_DiffuseIntensity;
uniform int gui_shading_type; // 0 - relief , 1 - parallax normal mapping , 2 - tessellation

// Global variables
vec3 Kd = vec3(gui_DiffuseIntensity);
vec3 Ks = vec3(gui_SpecularIntensity*0.5);
vec3 Ka = vec3(0.0);
float alpha = 30.0;

vec3 LightSource_diffuse  = vec3(1.0);
vec3 LightSource_ambient  = vec3(1.0);
vec3 LightSource_specular = vec3(1.0);

// input variables
in vec3 texcoord;

const int no_lights = 2;
in vec3 TSLightPosition[no_lights];
in vec3 TSViewDirection[no_lights];
in vec3 TSHalfVector;

// output color
out vec4 FragColor;

vec4 bump_mapping(int lightIndeks,vec2 texcoord){

    vec3  fvTSLightPosition  = normalize( TSLightPosition[lightIndeks] );
    vec3  fvESVertexNormal   = normalize( ( texture( texNormal, texcoord.xy ).xyz * 2.0 ) - 1.0 );

    if(!gui_bNormal) fvESVertexNormal = vec3(0,0,1);

    float fNDotL             = dot( fvESVertexNormal, fvTSLightPosition );

    vec3  fvReflection       = normalize( ( ( 2.0 * fvESVertexNormal ) * fNDotL ) - fvTSLightPosition );
    vec3  fvTSViewDirection  = normalize( TSViewDirection[lightIndeks] );
    float fRDotV             = max( 0.0, dot( fvReflection, fvTSViewDirection ) );

    vec4  fvBaseColor        = texture( texDiffuse, texcoord.xy );
    vec4  fvSpecularColor    = texture( texSpecular, texcoord.xy );
    vec4  fvSSAOColor        = texture( texSSAO, texcoord.xy );

    if(!gui_bDiffuse)  fvBaseColor = vec4(0.8); // some gray color
    if(!gui_bOcclusion)fvSSAOColor = vec4(1.0);

    vec4  fvTotalAmbient   = vec4(Ka * LightSource_ambient,1) * fvBaseColor;
    vec4  fvTotalDiffuse   = vec4(Kd * LightSource_diffuse,1) * fNDotL * fvBaseColor;
    vec4  fvTotalSpecular  = vec4(Ks * LightSource_specular,1) * ( pow( fRDotV, alpha ) ) * fvSpecularColor * 3;

    if(!gui_bSpecular) fvTotalSpecular = vec4(0);


    return ( fvTotalAmbient + fvTotalDiffuse*fvSSAOColor + fvTotalSpecular*fvSSAOColor );
}

vec2 parallax_normal_mapping(){
      vec2 newTexCoord;
      vec3 h = normalize(TSHalfVector);//half vector

      float scale = gui_depthScale*0.04;
      float bias  = -0.03;
      if (gui_bHeight)
      {
          float height = texture(texHeight, texcoord.st).r;

          height = height * scale + bias;
          newTexCoord = texcoord.st + (height * h.xy);
      }
      else
      {
          newTexCoord = texcoord.st;
      }
      return newTexCoord;
}



// Relief shader
in vec3 ESVertexPosition;
in vec3 ESVertexNormal;
in vec3 ESVertexTangent;
in vec3 ESVertexBitangent;


uniform  vec4  cameraPos;
uniform  vec4  lightPos;

float depth = 0.04*gui_depthScale;

// based on https://github.com/kiirala/reliefmap
vec2 relief_mapping() {
        if (gui_bHeight){
        vec3 N          = normalize(ESVertexNormal);
        vec3 eview      = normalize(ESVertexPosition);
        vec3 etangent   = normalize(ESVertexTangent);
        vec3 ebitangent = normalize(ESVertexBitangent);

        vec3 tview      = normalize(vec3(dot(eview, etangent), dot(eview, ebitangent), dot(eview, -N)));
        vec2 ds         = tview.xy * depth / tview.z;
        vec2 dp         = texcoord.xy;


        const int linear_steps = 15;
        const int binary_steps = 15;
        float depth_step = 1.0 / linear_steps;
        float size = depth_step;
        float depth = 1.0;
        float best_depth = 1.0;
        for (int i = 0 ; i < linear_steps - 1 ; ++i) {
                depth -= size;
                vec4 t = texture(texHeight, dp + ds * depth);
                if (depth >= 1.0 - t.r)
                        best_depth = depth;
        }
        depth = best_depth - size;
        for (int i = 0 ; i < binary_steps ; ++i) {
                size *= 0.5;
                vec4 t = texture(texHeight, dp + ds * depth);
                if (depth >= 1.0 - t.r) {
                        best_depth = depth;
                        depth -= 2 * size;
                }
                depth += size;
        }

        return dp + best_depth * ds;
        }else return texcoord.st;
}




void main( void )
{
    // calculate uv coords based on selected algorithm
    vec2 texcoords = texcoord.st;
    if(gui_shading_type == 0){
        texcoords = relief_mapping();
    }else if(gui_shading_type == 1){
        texcoords = parallax_normal_mapping();
    }

    // apply standarnd normal mapping
    FragColor = (bump_mapping(0,texcoords)+bump_mapping(1,texcoords))/2;

    //FragColor = vec4(texcoord,1);//texture( texDiffuse, texcoord.xy );

}
