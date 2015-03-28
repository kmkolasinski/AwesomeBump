#version 400 core
subroutine vec4 filterModeType();
subroutine uniform filterModeType filterMode;

uniform sampler2D layerA; // first layer
uniform sampler2D layerB; // second layer
uniform sampler2D layerC; // third layer
uniform sampler2D layerD; //
uniform sampler2D layerE; //
uniform sampler2D layerF; //
uniform sampler2D layerG; //

vec2 dxy = vec2(1.0/max(textureSize(layerA,0).r,textureSize(layerA,0).g));

in vec2 v2QuadCoords;
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normal_filter(){		
    vec4 c =  texture( layerA, v2QuadCoords.xy);
    return c;
}

// ----------------------------------------------------------------
//                        GAUSSIAN BLUR
// ----------------------------------------------------------------

float gaussian(vec2 pos,float w){
    return exp( (-(pos.x)*(pos.x) - (pos.y)*(pos.y))/(w*w+1.0) );
}

vec4 gauss_filter_h(sampler2D layer,float w, int radius, float depth){
    vec4 color  = vec4(0);
    float totalw = 0;
    int i = 0;
    for(int j = -radius ; j <= radius ; j++){
            float cw = gaussian(vec2(i,j),w);
            totalw += cw;
            color   += texture(layer,v2QuadCoords.xy+depth*vec2(i,j)*dxy)*cw;
    }

    return color/totalw;
}

vec4 gauss_filter_v(sampler2D layer,float w, int radius, float depth){
    vec4 color = vec4(0);
    float totalw = 0;
    int j = 0;
    for(int i = -radius ; i <= radius ; i++){
            float cw = gaussian(vec2(i,j),w);
            totalw += cw;
            color   += texture(layer,v2QuadCoords.xy+depth*vec2(i,j)*dxy)*cw;
    }
    return color/totalw;
}
uniform float gui_gauss_w;
uniform float gui_gauss_radius;
uniform int   gui_gauss_mode;

subroutine(filterModeType) vec4 mode_gauss_filter(){
    float w     = gui_gauss_w;
    float depth = 1.0;
    int radius = int(gui_gauss_radius);
    if(gui_gauss_mode == 0){
         return gauss_filter_h(layerA,w,radius,depth);
    }else{
         return gauss_filter_v(layerA,w,radius,depth);
    }
}

// ----------------------------------------------------------------
//                        BLOOM EFFECT
// ----------------------------------------------------------------

subroutine(filterModeType) vec4 mode_bloom_filter(){

   vec3 color = texture(layerA, v2QuadCoords.st).rgb;
   vec3 b1    = texture(layerB, v2QuadCoords.st).rgb;
   vec3 b2    = texture(layerC, v2QuadCoords.st).rgb;
   vec3 b3    = texture(layerD, v2QuadCoords.st).rgb;
   vec3 b4    = texture(layerE, v2QuadCoords.st).rgb;

   vec3 bloom =  (b1*0.4f + b2*0.8f + b3*0.6f + b4*.8f)/2.0;
   float x = v2QuadCoords.x;
   float y = v2QuadCoords.y;
   float attenuateX = 1.0 - 0.8*(1 - smoothstep(0.0,0.4,x) + smoothstep(0.6,1.0,x));
   float attenuateY = 1.0 - 0.8*(1 - smoothstep(0.0,0.4,y) + smoothstep(0.6,1.0,y));
   return vec4(  color + bloom * attenuateX * attenuateY  ,1);
}

out vec4 FragColor;
void main() {   
        FragColor   =  filterMode();
 }
