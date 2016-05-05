#version 330 core
//#extension GL_ARB_texture_query_levels : enable


// Uniform variables
uniform sampler2D texDiffuse;
uniform sampler2D texNormal;
uniform sampler2D texSpecular;
uniform sampler2D texHeight;
uniform sampler2D texSSAO;
uniform sampler2D texRoughness;
uniform sampler2D texMetallic;
uniform sampler2D texMaterial;

uniform samplerCube texDiffuseEnvMap;  // prefilltered diffuse cube map
uniform samplerCube texEnvMap;         // full cube map
uniform int num_mipmaps; // number of mipmaps

uniform bool gui_bSpecular;
uniform bool gui_bOcclusion;
uniform bool gui_bHeight;
uniform bool gui_bDiffuse;
uniform bool gui_bNormal;
uniform bool gui_bRoughness;
uniform bool gui_bMetallic;
uniform float gui_LightPower;
uniform float gui_LightRadius;

uniform int gui_noPBRRays;
uniform bool gui_bUseSimplePBR;
uniform bool gui_bMaterialsPreviewEnabled;
uniform bool gui_bShowTriangleEdges;

uniform  float gui_depthScale;
uniform float gui_SpecularIntensity;
uniform float gui_DiffuseIntensity;
uniform int gui_shading_type; // 0 - relief , 1 - parallax normal mapping , 2 - tessellation
uniform int gui_shading_model; // 0 - PBR , 1 - Bump mapping
uniform vec3 cameraPos;
uniform vec3 lightDirection;
// Global variables
vec3 Kd = vec3(gui_DiffuseIntensity);
vec3 Ks = vec3(gui_SpecularIntensity*0.5);
vec3 Ka = vec3(0.0);
float alpha =  1.0 / (gui_LightRadius*0.1 + 0.01);

vec3 LightSource_diffuse  = vec3(1.0);
vec3 LightSource_ambient  = vec3(1.0);
vec3 LightSource_specular = vec3(1.0);

// input variables
in vec3 texcoord;

const int no_lights = 2;
in vec3 TSLightPosition[no_lights];
in vec3 TSViewDirection[no_lights];
in vec3 TSHalfVector;

in mat3 TBN;
// output color
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragNormal;
layout(location = 2) out vec4 FragGlowColor;
layout(location = 3) out vec4 FragPosition;


// global variables
vec3  fvESVertexNormal;
vec4  fvBaseColor;
vec4  fvSpecularColor;
vec4  fvSSAOColor;
float fvGlossiness;
vec4  fvRoughness;
vec4  fvMetallic;

vec4 bump_mapping(int lightIndeks,vec2 texcoord){

    vec3  fvTSLightPosition  = normalize( TSLightPosition[lightIndeks] );


    float fNDotL             = max(dot( fvESVertexNormal, fvTSLightPosition ),0.0);

    vec3  fvReflection       = normalize( ( ( 2.0 * fvESVertexNormal ) * fNDotL ) - fvTSLightPosition );
    vec3  fvTSViewDirection  = normalize( TSViewDirection[lightIndeks] );

    float fRDotV             = max( 0.0, dot( fvReflection, fvTSViewDirection ) );



    vec4  fvTotalAmbient   = vec4(Ka * LightSource_ambient,1) * fvBaseColor;
    vec4  fvTotalDiffuse   = gui_LightPower * vec4(Kd * LightSource_diffuse,1) * fNDotL * fvBaseColor;

    vec3  lightDirection  = normalize( TSLightPosition[lightIndeks] );

    vec4  fvTotalSpecular  = gui_LightPower * vec4(Ks * LightSource_specular,1) * ( pow( fRDotV, alpha*fvGlossiness ) ) * fvSpecularColor * 3;



    if(!gui_bSpecular) fvTotalSpecular = vec4(0);


    return ( fvTotalDiffuse*fvSSAOColor + fvTotalSpecular);
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
in vec3 ESHalfVector;
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



in vec3 WSTangent;
in vec3 WSBitangent;
in vec3 WSNormal;
in vec3 WSPosition;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
vec3 normalizedLightDirection = normalize(lightDirection);

const float PI = 3.14159;
float chiGGX(float v)
{
    return (v > 0) ? 1.0 : 0.0;
}

float GGX_Distribution(vec3 n, vec3 h, float alpha)
{
    float NoH = dot(n,h);
    float alpha2 = alpha * alpha;
    float NoH2 = NoH * NoH;
    float den = NoH2 * alpha2 + (1 - NoH2);
    return (chiGGX(NoH) * alpha2) / ( PI * den * den );
}


float GGX_PartialGeometryTerm(vec3 v, vec3 n, vec3 h, float alpha2)
{
    float VoH2 = clamp(dot(v,h),0,1);
    float chi = chiGGX( VoH2 / clamp(dot(v,n),0,1) );
    VoH2 = VoH2 * VoH2;
    float tan2 = ( 1 - VoH2 ) / VoH2;
    return (chi * 2) / ( 1 + sqrt( 1 + alpha2 * tan2 ) );
}


float GGX_PartialGeometryTerm_OPT(float VdotH, float VdotN, float alpha2)
{
    float VoH2 = VdotH;//clamp(dot(v,h),0,1);
    float chi = chiGGX( VoH2 / VdotN );
    VoH2 = VoH2 * VoH2;
    float tan2 = ( 1 - VoH2 ) / VoH2;
    return (chi * 2) / ( 1 + sqrt( 1 + alpha2 * tan2 ) );
}



vec3 Fresnel_Schlick(float cosT, vec3 F0)
{
  return F0 + (1-F0)* pow( cosT, 4.0);
}


// randon number starting seed
ivec2 seed = ivec2(+64523*length(WSPosition.yz)*gl_FragCoord.y,
                   +62310*length(WSPosition.xz)*gl_FragCoord.x);


// Random number generator based on:
// http://www.reedbeta.com/blog/2013/01/12/quick-and-easy-gpu-random-numbers-in-d3d11/
vec2 wang_hash()
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed*(1.0 / 4294967296.0);
}

//float rand(){
//    return fract(sin(dot(v2QuadCoords.xy ,vec2(12.9898,78.233))) * 43758.5453);
//}

vec3 calc_random_vec(float roughness,vec3 up,vec3 right,vec3 dir){

    vec2 fseed = clamp(wang_hash(),vec2(0),vec2(1));


    float theta = atan(roughness*sqrt(fseed.x)/sqrt(1-fseed.x));
    float phi   = 4*PI*fseed.y;
    vec3 temp = cos(phi) * up + sin(phi) * right;
    return (cos(theta) * dir + sin(theta) * temp);
}

// Based on:
// http://www.codinglabs.net/article_physically_based_rendering_cook_torrance.aspx
vec4 PBR_Specular(float roughness,
                  vec3 F0,
                  inout vec3 kS,
                  samplerCube texEnvMap,
                  vec3 surfacePosition,
                  vec3 surfaceNormal,vec2 texcoords){



    vec3 v = normalize(cameraPos - surfacePosition);
    vec3 n = surfaceNormal; // approximated normal in world space
    //vec3 n = normalize(surfaceNormal);// face based normal
    vec3 l = normalize(reflect(-v,n));
    vec3 h = normalize(l + v);

    vec3 up     = vec3(0,1,0);
    vec3 right  = normalize(cross(up,l));
    up          = cross(l,right);

    vec3 radiance  = vec3(0);
    float  NdotV     = clamp(dot(n, v),0,1);


    //int no_mipmap = textureQueryLevels(texEnvMap); // get number of mipmaps

    int no_samples =  gui_noPBRRays;
    // do the lighting calculation for each fragment.

    float r2 = roughness * roughness;
    for(int i = 0 ; i < no_samples ; i++ ){
         vec3 lp =  calc_random_vec(r2,up,right,l);



         // Calculate the half vector
         vec3 halfVector = normalize(lp + v);


         float VdotH = clamp(dot( halfVector, v ),0.01,1.0);
         float HdotN = max(dot( halfVector, n ),0.01);
         float LdotN = max(dot( lp, n ),0.01);
         float LdotH = max(dot( lp, halfVector ),0.01);


         vec3 fresnel   = Fresnel_Schlick( (1-VdotH), F0 );

         // Geometry term
         float geometry = GGX_PartialGeometryTerm_OPT( VdotH, NdotV, r2)
                        * GGX_PartialGeometryTerm_OPT( LdotH, LdotN, r2);

         // Calculate the Cook-Torrance denominator
         float denominator = clamp( 4 * (NdotV * HdotN + 0.05) ,0,1);

         kS += fresnel ;


         // Accumulate the radiance
         float light = max(dot(normalizedLightDirection,lp),0.0);         
         light       = 1-exp(-pow((5*gui_LightRadius*light),4));

         vec3 color = texture( texEnvMap, lp ).rgb * exp(-0.1*gui_LightPower) + gui_LightPower * light *0.4;

         radiance +=  color  * geometry/denominator * fresnel ;

    }
    // Scale back for the samples count
    kS = clamp( kS / no_samples ,0,1);
    return vec4(radiance / no_samples,1);
}

vec4 PBR_Specular_SIMPLE(float roughness,
                  vec3 F0,
                  inout vec3 kS,
                  samplerCube texEnvMap,
                  vec3 surfacePosition,
                  vec3 surfaceNormal,vec2 texcoords){



    vec3 v = normalize(cameraPos - surfacePosition);
    vec3 n = surfaceNormal; // approximated normal in world space
    //vec3 n = normalize(surfaceNormal);// face based normal
    vec3 l = normalize(reflect(-v,n));
    vec3 h = normalize(l + v);

    vec3 up     = vec3(0,1,0);
    vec3 right  = normalize(cross(up,l));
    up          = cross(l,right);

    vec3 radiance  = vec3(0);
    float  NdotV     = clamp(dot(n, v),0,1);


    int no_mipmap = num_mipmaps;
    //textureQueryLevels(texEnvMap); // get number of mipmaps

    int no_samples =  1;
    // do the lighting calculation for each fragment.

    float r2 = roughness * roughness;

    vec3 lp =  l;
    // Calculate the half vector
    vec3 halfVector = normalize(lp + v);


    float VdotH = max(dot( halfVector, v ),0.01);
    float HdotN = max(dot( halfVector, n ),0.01);
    float LdotN = max(dot( lp, n ),0.01);
    float LdotH = max(dot( lp, halfVector ),0.01);


    vec3 fresnel   = Fresnel_Schlick( VdotH, F0 );

    // Geometry term
    float geometry = GGX_PartialGeometryTerm_OPT( VdotH, NdotV, r2)
                   * GGX_PartialGeometryTerm_OPT( LdotH, LdotN, r2);

    // Calculate the Cook-Torrance denominator
    float denominator = clamp( 4 * (NdotV * HdotN + 0.05) ,0,1);

    kS  = fresnel ;
    float light = max(dot(normalizedLightDirection,lp),0.0);
    light       = 1-exp(-pow((5*gui_LightRadius*light),4));
    // Accumulate the radiance
    vec3 color = texture( texEnvMap, lp , roughness *  no_mipmap ).rgb * exp(-0.1*gui_LightPower) + gui_LightPower * light *0.4;

    radiance =  color  * geometry * fresnel / denominator;// * sinT;

    // Scale back for the samples count
    kS = clamp( kS ,0,1);
    return vec4(radiance ,1);
}



void main( void )
{

    // Triangle edges:
    if(gui_bShowTriangleEdges){
        if(gui_bMaterialsPreviewEnabled) FragColor = texture(texDiffuse, texcoord.st);
        else FragColor     = vec4(0);
        FragColor.a   = 1;
        FragNormal    = vec4(0);
        FragGlowColor = vec4(0);
        FragPosition  = vec4(10);
        return;
    }

    if(gui_bMaterialsPreviewEnabled){
        FragColor     = texture(texMaterial, texcoord.st);
        FragNormal    = vec4(0);
        FragGlowColor = vec4(0);
        FragPosition  = vec4(0);
        return;
    }

    // calculate uv coords based on selected algorithm
    vec2 texcoords = texcoord.st;
    if(gui_shading_type == 0){
        texcoords = relief_mapping();
    }else if(gui_shading_type == 1){
        texcoords = parallax_normal_mapping();
    }

    // setting up global variables
    fvESVertexNormal   = normalize( ( texture( texNormal, texcoords.xy ).xyz * 2.0 ) - 1.0 );
    if(!gui_bNormal)     fvESVertexNormal = vec3(0,0,1);

    fvBaseColor        = texture( texDiffuse, texcoords.xy );
    fvSpecularColor    = texture( texSpecular, texcoords.xy );
    fvSSAOColor        = texture( texSSAO, texcoords.xy );
    fvRoughness        = texture( texRoughness, texcoords.xy );
    fvMetallic         = texture( texMetallic, texcoords.xy );
    fvGlossiness       = clamp(fvRoughness.r,0.0,1.0)*100+1.0;

    if(!gui_bRoughness) fvGlossiness      = 1.0;

    if(!gui_bDiffuse)  fvBaseColor     = vec4(0.8); // some gray color
    if(!gui_bOcclusion)fvSSAOColor     = vec4(1.0);
    if(!gui_bSpecular) fvSpecularColor = vec4(0);
    // TODO: add here R and M control

    vec3  ts_normal   = fvESVertexNormal;
    mat3  iTBN        = transpose(TBN);
    vec3 snormal      = iTBN*normalize(vec3(ts_normal.x,ts_normal.y,5*ts_normal.z));
    vec3 surfaceNormal= normalize(snormal); // approximated normal in world space

    vec4 finalColor = vec4(0);

    if(gui_shading_model == 1){
    // apply standarnd normal mapping

        vec3 irradiance = texture(texDiffuseEnvMap, normalize(surfaceNormal)).rgb;
        vec3 diffuse    = fvBaseColor.rgb * irradiance ;
        fvBaseColor.rgb =  diffuse;
        vec4 bumpMapShadingColor = (bump_mapping(0,texcoords)+bump_mapping(1,texcoords))/2;
        //bumpMapShadingColor = bump_mapping(0,texcoords);
        FragColor = bumpMapShadingColor;
        finalColor = FragColor ;

    }else{ // in case of PBR model

        // PBR calculations
        vec3  materialColour = fvBaseColor.rgb;
        vec4  aoColour       = fvSSAOColor;
        float roughness      = clamp(fvRoughness.r,0,1);
        vec3  metallicColour = clamp(fvMetallic.rgb,vec3(0),vec3(1));

        if(!gui_bRoughness) roughness      = 0.0;
        if(!gui_bMetallic)  metallicColour = vec3(0.5);

        vec3 F0 = vec3(metallicColour);
        vec3 kS = vec3(0);

        vec4 specular = vec4(0);

        // in case of simple calculation use mipmaps instead integration
        if(gui_bUseSimplePBR)
        {
            specular =     PBR_Specular_SIMPLE(roughness,
                                       F0,kS,
                                       texEnvMap,
                                       WSPosition,
                                       surfaceNormal,texcoords);
        }else{
             specular =    PBR_Specular(roughness,
                                        F0,kS,
                                        texEnvMap,
                                        WSPosition,
                                        surfaceNormal,texcoords);
        }

        vec3 kD = (1 - kS) ;
        if(!gui_bSpecular) fvSpecularColor = vec4(1.0);
        // Calculate the diffuse contribution
        float NdotL = max(dot(surfaceNormal,normalizedLightDirection),0.0);

        vec3 irradiance = texture(texDiffuseEnvMap, normalize(surfaceNormal)).rgb * exp(-0.1*gui_LightPower) + vec3(NdotL * gui_LightPower *0.4);
        vec3 diffuse    = materialColour * irradiance ;

        FragColor  =  gui_DiffuseIntensity  * vec4(kD * diffuse,1) * aoColour
                   +  gui_SpecularIntensity * fvSpecularColor * vec4(materialColour,1) * ( specular ) ;

        finalColor = FragColor ;
    }

    // Final colors
    FragNormal    = vec4(WSPosition,1);

    float bloomLevel = 0.5;
    float level      = dot(finalColor.rgb,finalColor.rgb)/3;
    FragGlowColor = vec4(0);
    if(level > bloomLevel )FragGlowColor = finalColor;

    FragPosition = vec4(ESVertexPosition,1);



}
