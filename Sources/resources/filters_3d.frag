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

float width  = textureSize(layerA,0).r; //texture width
float height = textureSize(layerA,0).g; //texture height
vec2 texel = vec2(1.0/width,1.0/height);
vec2 dxy   = vec2(1.0/max(width,height));

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



//uniform variables from external script

float focalDepth = 10.0;  //focal distance value in meters, but you may use autofocus option below
float focalLength = 25.0; //focal length in mm
float fstop = 140.0;       //f-stop value
bool showFocus = false; //show debug focus point and focal range (red = focal point, green = focal range)

#define PI 3.14159265

//------------------------------------------
//user variables

int samples = 4; //samples on the first ring
int rings   = 10; //ring count

bool manualdof = false; //manual dof calculation
float ndofstart = 1.0; //near dof blur start
float ndofdist = 2.0; //near dof blur falloff distance
float fdofstart = 1.0; //far dof blur start
float fdofdist = 3.0; //far dof blur falloff distance

float CoC = 0.03;//circle of confusion size in mm (35mm film = 0.03mm)

bool vignetting = false; //use optical lens vignetting?
float vignout = 1.3; //vignetting outer border
float vignin = 0.0; //vignetting inner border
float vignfade =22.0; //f-stops till vignete fades

bool autofocus = true; //use autofocus in shader? disable if you use external focalDepth value
vec2 focus     = vec2(0.5,0.5); // autofocus point on screen (0.0,0.0 - left lower corner, 1.0,1.0 - upper right)
float maxblur  = 1.0; //clamp value of max blur (0.0 = no blur,1.0 default)

float threshold = 0.25; //highlight threshold;
float gain      = 1.0; //highlight gain;

float bias   = 0.25; //bokeh edge bias 0.5
float fringe = 0.7; //bokeh chromatic aberration/fringing

bool noise = false; //use noise instead of pattern for sample dithering
float namount = 0.0001; //dither amount

bool depthblur = true; //blur the depth buffer?
float dbsize = 1.25; //depthblursize

/*
next part is experimental
not looking good with small sample and ring count
looks okay starting from samples = 4, rings = 4
*/

bool pentagon = false; //use pentagon as bokeh shape?
float feather = 0.4; //pentagon shape feather

//------------------------------------------


float penta(vec2 coords) //pentagonal shape
{
        float scale = float(rings) - 1.3;
        vec4  HS0 = vec4( 1.0,         0.0,         0.0,  1.0);
        vec4  HS1 = vec4( 0.309016994, 0.951056516, 0.0,  1.0);
        vec4  HS2 = vec4(-0.809016994, 0.587785252, 0.0,  1.0);
        vec4  HS3 = vec4(-0.809016994,-0.587785252, 0.0,  1.0);
        vec4  HS4 = vec4( 0.309016994,-0.951056516, 0.0,  1.0);
        vec4  HS5 = vec4( 0.0        ,0.0         , 1.0,  1.0);

        vec4  one = vec4( 1.0 );

        vec4 P = vec4((coords),vec2(scale, scale));

        vec4 dist = vec4(0.0);
        float inorout = -4.0;

        dist.x = dot( P, HS0 );
        dist.y = dot( P, HS1 );
        dist.z = dot( P, HS2 );
        dist.w = dot( P, HS3 );

        dist = smoothstep( -feather, feather, dist );

        inorout += dot( dist, one );

        dist.x = dot( P, HS4 );
        dist.y = HS5.w - abs( P.z );

        dist = smoothstep( -feather, feather, dist );
        inorout += dist.x;

        return clamp( inorout, 0.0, 1.0 );
}

vec3 bdepth(vec2 coords) //blurring depth
{
        vec3 d = vec3(0.0);
        float kernel[9];
        vec2 offset[9];

        vec2 wh = vec2(texel.x, texel.y) * dbsize;

        offset[0] = vec2(-wh.x,-wh.y);
        offset[1] = vec2( 0.0, -wh.y);
        offset[2] = vec2( wh.x -wh.y);

        offset[3] = vec2(-wh.x,  0.0);
        offset[4] = vec2( 0.0,   0.0);
        offset[5] = vec2( wh.x,  0.0);

        offset[6] = vec2(-wh.x, wh.y);
        offset[7] = vec2( 0.0,  wh.y);
        offset[8] = vec2( wh.x, wh.y);

        kernel[0] = 1.0/16.0;   kernel[1] = 2.0/16.0;   kernel[2] = 1.0/16.0;
        kernel[3] = 2.0/16.0;   kernel[4] = 4.0/16.0;   kernel[5] = 2.0/16.0;
        kernel[6] = 1.0/16.0;   kernel[7] = 2.0/16.0;   kernel[8] = 1.0/16.0;


        for( int i=0; i<9; i++ )
        {
                vec3 tmp = texture2D(layerB, coords + offset[i]).xyz;
                d += tmp * kernel[i];
        }

        return d;
}


vec3 color(vec2 coords,float blur) //processing the sample
{
        vec3 col = vec3(0.0);

        col.r = texture2D(layerA,coords + vec2(0.0,1.0)*texel*fringe*blur).r;
        col.g = texture2D(layerA,coords + vec2(-0.866,-0.5)*texel*fringe*blur).g;
        col.b = texture2D(layerA,coords + vec2(0.866,-0.5)*texel*fringe*blur).b;

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

vec3 debugFocus(vec3 col, float blur, float depth)
{
        float edge = 0.002*depth; //distance based edge smoothing
        float m = clamp(smoothstep(0.0,edge,blur),0.0,1.0);
        float e = clamp(smoothstep(1.0-edge,1.0,blur),0.0,1.0);

        col = mix(col,vec3(1.0,0.5,0.0),(1.0-m)*0.6);
        col = mix(col,vec3(0.0,0.5,1.0),((1.0-e)-(1.0-m))*0.2);

        return col;
}

float linearize(vec3 position)
{
        return length(position.xyz);
}

float vignette()
{
        float dist = distance(v2QuadCoords.xy, vec2(0.5,0.5));
        dist = smoothstep(vignout+(fstop/vignfade), vignin+(fstop/vignfade), dist);
        return clamp(dist,0.0,1.0);
}

// ----------------------------------------------------------------
//                        DOF/BOKEH EFFECT
// ----------------------------------------------------------------

subroutine(filterModeType) vec4 mode_dof_filter(){
        //scene depth calculation

        float depth = linearize(texture2D(layerB,v2QuadCoords.xy).xyz);

        if (depthblur)
        {
                depth = linearize(bdepth(v2QuadCoords.xy));
        }

        //focal plane calculation

        float fDepth = focalDepth;

        if (autofocus)
        {
                fDepth = linearize(texture2D(layerB,focus).xyz);
        }

        //dof blur factor calculation

        float blur = 0.0;

        if (manualdof)
        {
                float a = depth-fDepth; //focal plane
                float b = (a-fdofstart)/fdofdist; //far DoF
                float c = (-a-ndofstart)/ndofdist; //near Dof
                blur = (a>0.0)?b:c;
        }

        else
        {
                float f = focalLength; //focal length in mm
                float d = fDepth*1000.0; //focal plane in mm
                float o = depth*1000.0; //depth in mm

                float a = (o*f)/(o-f);
                float b = (d*f)/(d-f);
                float c = (d-f)/(d*fstop*CoC);

                blur = abs(a-b)*c;
        }

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
                col = texture2D(layerA, v2QuadCoords.xy).rgb;
        }

        else
        {
                col = texture2D(layerA, v2QuadCoords.xy).rgb;
                float s = 1.0;
                int ringsamples;

                for (int i = 1; i <= rings; i += 1)
                {
                        ringsamples = i * samples;

                        for (int j = 0 ; j < ringsamples ; j += 1)
                        {
                                float step = PI*2.0 / float(ringsamples);
                                float pw = (cos(float(j)*step)*float(i));
                                float ph = (sin(float(j)*step)*float(i));
                                float p = 1.0;
                                if (pentagon)
                                {
                                        p = penta(vec2(pw,ph));
                                }
                                col += color(v2QuadCoords.xy + vec2(pw*w,ph*h),blur)*mix(1.0,(float(i))/(float(rings)),bias)*p;
                                s += 1.0*mix(1.0,(float(i))/(float(rings)),bias)*p;
                        }
                }
                col /= s; //divide by sample count
        }

        if (showFocus)
        {
                col = debugFocus(col, blur, depth);
        }

        if (vignetting)
        {
                col *= vignette();
        }

        return vec4(col,1);
}


out vec4 FragColor;
void main() {   
        FragColor   =  filterMode();
 }
