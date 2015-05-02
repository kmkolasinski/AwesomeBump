
/*
#version 400 core
subroutine vec4 filterModeType();
subroutine uniform filterModeType filterMode;
*/

uniform sampler2D layerA; // first layer
uniform sampler2D layerB; // second layer
uniform sampler2D layerC; // third layer
uniform sampler2D layerD; //
uniform sampler2D layerE; //
uniform sampler2D layerF; //
uniform sampler2D layerG; //

float width  = textureSize(layerA,0).r; //texture width
float height = textureSize(layerA,0).g; //texture height
vec2 texel = vec2(1.0/width,1.0/height);
vec2 dxy   = vec2(1.0/max(width,height));

in vec2 v2QuadCoords;

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
#ifdef NORMAL_FILTER
vec4 filter(){
    vec4 c = texture( layerA, v2QuadCoords.xy);
    return c;
}
#endif


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

#ifdef GAUSSIAN_BLUR_FILTER

uniform float gui_gauss_w;
uniform float gui_gauss_radius;
uniform int   gui_gauss_mode;

vec4 filter(){
    float w     = gui_gauss_w;

    float depth = 2.0;
    int radius = int(gui_gauss_radius);
    if(gui_gauss_mode == 0){
         return gauss_filter_h(layerA,w,radius,depth);
    }else{
         return gauss_filter_v(layerA,w,radius,depth);
    }


}

#endif


#ifdef BLOOM_FILTER
vec4 filter(){

   vec3 color = texture(layerA, v2QuadCoords.st).rgb;
   vec3 b1    = texture(layerB, v2QuadCoords.st).rgb;
   vec3 b2    = texture(layerC, v2QuadCoords.st).rgb;
   vec3 b3    = texture(layerD, v2QuadCoords.st).rgb;
   vec3 b4    = texture(layerE, v2QuadCoords.st).rgb;

   vec3 bloom =  (b1*0.4f + b2*0.8f + b3*0.6f + b4*0.8f)/2.5;
   float x = v2QuadCoords.x;
   float y = v2QuadCoords.y;
   float attenuateX = 1.0 - 0.8*(1 - smoothstep(0.0,0.4,x) + smoothstep(0.6,1.0,x));
   float attenuateY = 1.0 - 0.8*(1 - smoothstep(0.0,0.4,y) + smoothstep(0.6,1.0,y));

   return vec4(  color + bloom * attenuateX * attenuateY  ,1);
}
#endif

#ifdef TONE_MAPPING_FILTER

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 1.2;

vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec4 filter(){

   vec3 texColor = texture(layerA, v2QuadCoords.st).rgb;
   vec3 bloom = texture(layerB, vec2(0.5,0.5)).rgb;


   float ExposureBias = 1+0.5*length(bloom);
   vec3 curr = Uncharted2Tonemap(ExposureBias*texColor);

   vec3 whiteScale = 1.0f/Uncharted2Tonemap(vec3(W));
   vec3 color = curr*whiteScale;

   vec3 retColor = pow(color,vec3(2.5/2.2));

   return vec4(retColor,1);


}
#endif


#ifdef LENS_FLARES_FILTER

int uSamples = 4;
float uDispersal  = 0.5;
float uHaloWidth  = 0.5;
float uDistortion = 1.5;

/**

Lens flares effect based on:
www: http://john-chapman-graphics.blogspot.com/2013/02/pseudo-lens-flare.html
author: John Chapman

Copyright (c) 2013 John Chapman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
*/



/*----------------------------------------------------------------------------*/
vec4 textureDistorted(
        in sampler2D tex,
        in vec2 texcoord,
        in vec2 direction,
        in vec3 distortion
) {
        return vec4(
                texture(tex, texcoord + direction * distortion.r).r,
                texture(tex, texcoord + direction * distortion.g).g,
                texture(tex, texcoord + direction * distortion.b).b,
                1.0
        );
}

/*----------------------------------------------------------------------------*/
vec4 lf_scale = vec4(4.0);
vec4 lf_bias  = vec4(-0.5);
uniform int lf_step;
uniform mat4 lf_starMatrix;
vec4 filter() {
    if(lf_step == 0){
        // treshold image
        float dp = length(texture(layerB, v2QuadCoords.st).rgb)/sqrt(3.0);
        return max(vec4(0.0),  vec4(dp + lf_bias)) * lf_scale;
    }else if(lf_step == 1){ // prepare ghosts and halos

        vec2 texcoord = -v2QuadCoords + vec2(1.0); // flip texcoordoords
        vec2 texelSize = 1.0 / vec2(textureSize(layerB, 0));

        vec2 ghostVec = (vec2(0.5) - texcoord) * uDispersal;
        vec2 haloVec = normalize(ghostVec) * uHaloWidth;

        vec3 distortion = vec3(-texelSize.x * uDistortion, 0.0, texelSize.x * uDistortion);

// sample ghosts:
        vec4 result = vec4(0.0);
        float nw  = 0.0;
        for (int i = 0; i < uSamples; ++i) {
                vec2 offset = fract(texcoord + ghostVec * float(i));

                float weight = length(vec2(0.5) - offset) / length(vec2(0.5));


                result += textureDistorted(
                        layerB,
                        offset,
                        normalize(ghostVec),
                        distortion
                ) ;//* weight;

                nw += 1.0;//weight;
        }

        result *= texture(layerC, vec2(length(vec2(0.5) - texcoord) / length(vec2(0.5)),0.0));

//	sample halo:
        float weight = 2*length(vec2(0.5) - fract(texcoord + haloVec)) / length(vec2(0.5));
        weight = clamp(pow(clamp(1.0 - weight,0,1), 4.0),0,1);


        result += textureDistorted(
                layerB,
                fract(texcoord + haloVec),
                normalize(ghostVec),
                distortion
        )*weight ;
        nw += weight;

        return result/nw;

     }else{ // blend images
        vec2 texelSize = vec2(textureSize(layerB, 0));
        float ratio = texelSize.x/texelSize.y;
        vec4 dirtColor = texture(layerC, v2QuadCoords.st*vec2(0.5*ratio,1)); //dirt texture


        vec2 lensStarTexcoord = (lf_starMatrix * vec4(v2QuadCoords.st,1.0,0.0)).xy; // star texture
        lensStarTexcoord -= 0.5;
        lensStarTexcoord *= 0.5;
        lensStarTexcoord += 0.5;
        vec4 starColor = texture(layerD, lensStarTexcoord);
        vec4 lensColor = texture(layerB, v2QuadCoords.st);
        //return lensColor;
        return texture(layerA, v2QuadCoords.st) + 5*(starColor+dirtColor)*lensColor;
    }
}

#endif

// ----------------------------------------------------------------
//                        DOF EFFECT
// ----------------------------------------------------------------
/*
page:  http://blenderartists.org/forum/showthread.php?237488-GLSL-depth-of-field-with-bokeh-v2-4-(update)
DoF with bokeh GLSL shader v2.4
by Martins Upitis (martinsh) (devlog-martinsh.blogspot.com)

----------------------
The shader is Blender Game Engine ready, but it should be quite simple to adapt for your engine.

This work is licensed under a Creative Commons Attribution 3.0 Unported License.
So you are free to share, modify and adapt it for your needs, and even use it for commercial use.
I would also love to hear about a project you are using it.

Have fun,
Martins
----------------------

changelog:

2.4:
- physically accurate DoF simulation calculated from "focalDepth" ,"focalLength", "f-stop" and "CoC" parameters.
- option for artist controlled DoF simulation calculated only from "focalDepth" and individual controls for near and far blur
- added "circe of confusion" (CoC) parameter in mm to accurately simulate DoF with different camera sensor or film sizes
- cleaned up the code
- some optimization

2.3:
- new and physically little more accurate DoF
- two extra input variables - focal length and aperture iris diameter
- added a debug visualization of focus point and focal range

2.1:
- added an option for pentagonal bokeh shape
- minor fixes

2.0:
- variable sample count to increase quality/performance
- option to blur depth buffer to reduce hard edges
- option to dither the samples with noise or pattern
- bokeh chromatic aberration/fringing
- bokeh bias to bring out bokeh edges
- image thresholding to bring out highlights when image is out of focus

*/

#ifdef DOF_FILTER

const float focalDepth = 10.0;  //focal distance value in meters, but you may use autofocus option below
const float focalLength = 25.0; //focal length in mm
const float fstop = 140.0;       //f-stop value

#define PI 3.14159265

//------------------------------------------
//user variables

const int samples = 4; //samples on the first ring
const int rings   = 7; //ring count


const float CoC = 0.03;//circle of confusion size in mm (35mm film = 0.03mm)

const vec2 focus     = vec2(0.5,0.5); // autofocus point on screen (0.0,0.0 - left lower corner, 1.0,1.0 - upper right)
const float maxblur  = 1.0; //clamp value of max blur (0.0 = no blur,1.0 default)

const float threshold = 0.25; //highlight threshold;
const float gain      = 1.0; //highlight gain;

const float bias   = 0.25; //bokeh edge bias 0.5
const float fringe = 0.7; //bokeh chromatic aberration/fringing

const bool noise = false; //use noise instead of pattern for sample dithering
const float namount = 0.0001; //dither amount




vec3 color(vec2 coords,float blur) //processing the sample
{
        vec3 col = vec3(0.0);

        col.r = texture(layerA,coords + vec2(0.0,1.0)*texel*fringe*blur).r;
        col.g = texture(layerA,coords + vec2(-0.866,-0.5)*texel*fringe*blur).g;
        col.b = texture(layerA,coords + vec2(0.866,-0.5)*texel*fringe*blur).b;

        vec3 lumcoeff = vec3(0.299,0.587,0.114);
        float lum = dot(col.rgb, lumcoeff);
        float thresh = max((lum-threshold)*gain, 0.0);
        return col+mix(vec3(0.0),col,thresh*blur);
}

vec2 rand(vec2 coord) //generating noise/pattern texture for dithering
{
        float noiseX = ((fract(1.0-coord.s*(width/2.0))*0.25)+(fract(coord.t*(height/2.0))*0.75))*2.0-1.0;
        float noiseY = ((fract(1.0-coord.s*(width/2.0))*0.75)+(fract(coord.t*(height/2.0))*0.25))*2.0-1.0;

        if (noise)
        {
                noiseX = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0-1.0;
                noiseY = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0-1.0;
        }
        return vec2(noiseX,noiseY);
}


float linearize(vec3 position)
{
        return length(position.xyz);
}


// ----------------------------------------------------------------
//                        DOF/BOKEH EFFECT
// ----------------------------------------------------------------

vec4 filter(){
        //scene depth calculation


        float depth = linearize(texture(layerB,v2QuadCoords.xy).xyz);

        //focal plane calculation

        float fDepth = focalDepth;


        fDepth = linearize(texture(layerB,focus).xyz);


        //dof blur factor calculation

        float blur = 0.0;


        float f = focalLength; //focal length in mm
        float d = fDepth*1000.0; //focal plane in mm
        float o = depth*1000.0; //depth in mm

        float a = (o*f)/(o-f);
        float b = (d*f)/(d-f);
        float c = (d-f)/(d*fstop*CoC);

        blur = abs(a-b)*c;


        blur = clamp(blur,0.0,1.0);

        // calculation of pattern for ditering

        vec2 noise = rand(v2QuadCoords.xy)*namount*blur;

        // getting blur x and y step factor

        float w = (1.0/width)*blur*maxblur+noise.x;
        float h = (1.0/height)*blur*maxblur+noise.y;

        // calculation of final color

        vec3 col = vec3(0.0);


        if(blur < 0.05) //some optimization thing
        {
                col = texture(layerA, v2QuadCoords.xy).rgb;
        }else{
                col = texture(layerA, v2QuadCoords.xy).rgb;
                float s = 1.0;
                int ringsamples;

                for (int i = 1; i <= rings; i += 1)
                {

                        ringsamples = i * samples;
                        float step = PI*2.0 / float(ringsamples);
                        float rr = i/float(rings);
                        float mb = mix(1.0,rr,bias);
                        for (int j = 0 ; j < ringsamples ; j += 1)
                        {

                                float pw = (cos(float(j)*step)*float(i));
                                float ph = (sin(float(j)*step)*float(i));

                                col += color(v2QuadCoords.xy + vec2(pw*w,ph*h),blur)*mb;
                                s += mb;
                        }

                }


                col /= s; //divide by sample count

        }


        return vec4(col,1);

}

#endif

out vec4 FragColor;
void main() {
       FragColor   =  filter();
}

