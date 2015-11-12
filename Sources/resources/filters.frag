#ifndef USE_OPENGL_330
subroutine vec4 filterModeType();
subroutine uniform filterModeType filterMode;
#endif


uniform sampler2D layerA; // first layer
uniform sampler2D layerB; // second layer
uniform sampler2D layerC; // third layer
uniform sampler2D layerD; // fourth layer
uniform sampler2D materialTexture; // texture with material mask

uniform int quad_draw_mode;

uniform int gauss_mode;
uniform int gui_gauss_mask;
uniform	float gui_gauss_w;
uniform	int gui_gauss_radius;
uniform bool gui_gauss_show_mask;
uniform bool gui_gauss_invert_mask;
uniform float gui_gauss_blending;

uniform float gui_depth;
uniform float gui_small_details;
uniform float gui_medium_details;
uniform int gui_image_type;
uniform int gui_sharpen_blur;
uniform vec3 gui_inverted_components;
uniform float gui_hn_conversion_depth;
uniform vec3 hn_min_max_scale;
uniform vec3 min_color;
uniform vec3 max_color;
uniform float gui_normals_step;
uniform float gui_basemap_amp;
uniform int gui_sobel_combine;
uniform float gui_normal_flatting;
uniform int gui_filter_radius;
uniform int gui_combine_normals;
uniform float gui_mix_normals;
uniform float gui_smooth_radius;
uniform float gui_blend_normals;

uniform int gui_compressed_type;
uniform int gui_mode_dgaussian;

uniform int   gui_ssao_no_iters;
uniform float gui_ssao_depth;
uniform float gui_ssao_bias;
uniform float gui_ssao_intensity;

uniform int gui_clear_alpha;
uniform int gui_perspective_mode;
in vec2 v2QuadCoords;
vec2 dxy = vec2(1.0/max(textureSize(layerA,0).r,textureSize(layerA,0).g));

// position of corners used in perspective projection correction
uniform vec2 corner1;
uniform vec2 corner2;
uniform vec2 corner3;
uniform vec2 corner4;

mat3 sobel_kernel  = mat3(
                    -1.0, 0.0, +1.0,
                    -2.0, 0.0, +2.0,
                    -1.0, 0.0, +1.0
                            );

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------

#ifndef mode_normal_filter_330

#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_normal_filter(){
#else
vec4 ffilter(){
#endif
    vec4 c =  texture( layerA, v2QuadCoords.xy);    
    return c;
}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------

// taken from: http://axonflux.com/handy-rgb-to-hsl-and-rgb-to-hsv-color-model-c
/**
 * Converts an RGB color value to HSV. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSV_color_space.
 * Assumes r, g, and b are contained in the set [0, 255] and
 * returns h, s, and v in the set [0, 1].
 *
 * @param   Number  r       The red color value
 * @param   Number  g       The green color value
 * @param   Number  b       The blue color value
 * @return  Array           The HSV representation
 */
vec3 rgbToHsv(float r,float g,float b){
    //r = r/255, g = g/255, b = b/255;
    float max = max(max(r, g), b);
    float min = min(min(r, g), b);
    float h, s, v = max;

    float d = max - min;
    s = (max == 0.0) ? 0.0 : d / max;

    if(max == min){
        h = 0; // achromatic
    }else{
        if(max == r){
             h = (g - b) / d + (g < b ? 6.0 : 0.0);
        }else if(max == g){
             h = (b - r) / d + 2;
        }else{
             h = (r - g) / d + 4;
        }

        h /= 6.0;
    }

    return vec3(h, s, v);
}

/**
 * Converts an HSV color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSV_color_space.
 * Assumes h, s, and v are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 255].
 *
 * @param   Number  h       The hue
 * @param   Number  s       The saturation
 * @param   Number  v       The value
 * @return  Array           The RGB representation
 */
vec3 hsvToRgb(float h, float s,float v){
    float r, g, b;

    int   i = int(floor(h * 6));
    float f = h * 6.0 - i;
    float p = v * (1.0 - s);
    float q = v * (1.0 - f * s);
    float t = v * (1.0 - (1.0 - f) * s);

    if( i%6 == 0){
        r = v; g = t; b = p;
    }else if( i%6 == 1){
        r = q; g = v; b = p;
    }else if( i%6 == 2){
        r = p; g = v; b = t;
    }else if( i%6 == 3){
        r = p; g = q; b = v;
    }else if( i%6 == 4){
        r = t; g = p; b = v;
    }else if( i%6 == 5){
        r = v; g = p; b = q;
    }

    return vec3(r,g,b);
}



uniform float gui_hue; // number from -1:1
#ifndef mode_color_hue_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_color_hue_filter(){
#else
vec4 ffilter(){
#endif

   vec4 textureColor = texture( layerA, v2QuadCoords.xy);
   vec3 hsv = rgbToHsv(textureColor.r,textureColor.g,textureColor.b);
   hsv.r +=  (gui_hue);
   vec3 rgb = clamp(hsvToRgb(hsv.r,hsv.g,hsv.b),vec3(-1.0),vec3(1.0));
   return vec4(clamp(rgb,vec3(0),vec3(1)),1);

}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------


uniform vec4 corners_weights;
uniform int uv_scaling_mode;

float phi1(vec2 pos){return (1-pos.x)*(1-pos.y);}
float phi2(vec2 pos){return (pos.x)*(1-pos.y);}
float phi3(vec2 pos){return (pos.x)*(pos.y);}
float phi4(vec2 pos){return (1-pos.x)*(pos.y);}

#ifndef mode_perspective_transform_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_perspective_transform_filter(){
#else
vec4 ffilter(){
#endif
    vec2 transf_pos = v2QuadCoords.xy;
    vec4 color      = vec4(1);

    if(uv_scaling_mode == 1){
        vec2 texcoord = corners_weights.st;
        // calculating proper mapping for UV coordinates. I used here pow(x,alpha) function
        if(corners_weights.x >= 0){
            texcoord.x = pow(v2QuadCoords.x,(corners_weights.x+1));
        }else{
            texcoord.x = 1+pow(1-v2QuadCoords.x,abs(corners_weights.x)+1);
            texcoord.x = 1 - texcoord.x;
        }
        if(corners_weights.y >= 0){
            texcoord.y = pow(v2QuadCoords.y,(corners_weights.y+1));
        }else{
            texcoord.y = 1+pow(1-v2QuadCoords.y,abs(corners_weights.y)+1);
            texcoord.y = 1 - texcoord.y;
        }
        return  texture( layerA, texcoord );
    }

    // 1.
    if(gui_perspective_mode == 0){
    float p1 = phi1(v2QuadCoords.xy);
    float p2 = phi2(v2QuadCoords.xy);
    float p3 = phi3(v2QuadCoords.xy);
    float p4 = phi4(v2QuadCoords.xy);
    float sum = p1 + p2 + p3 + p4;

    transf_pos  = p1*corner1
                + p2*corner2
                + p3*corner3
                + p4*corner4;

    color       = texture( layerA, transf_pos.xy );


    // 2.
    }else if(gui_perspective_mode == 1){
    vec2 dc1 = corner1 + 0.0001 - vec2(0,0);
    vec2 dc2 = corner2 - vec2(1,0);
    vec2 dc3 = corner3 - vec2(1,1);
    vec2 dc4 = corner4 - vec2(0,1);

    vec2 r0  = vec2(0,0) - vec2(dc1.x,dc1.y);
    vec2 r1  = vec2(1,0) - vec2(dc2.x,dc2.y);
    vec2 r2  = vec2(1,1) - vec2(dc3.x,dc3.y);
    vec2 r3  = vec2(0,1) - vec2(dc4.x,dc4.y);
    vec2 r10 = r1-r0;
    vec2 r30 = r3-r0;
    vec2 r23 = r2-r3;
    vec2 p0  = v2QuadCoords;
    vec2 alpha = p0-r0;
    vec2 delta = r23-r10;
    float cc = alpha.x*r30.y-alpha.y*r30.x;
    float bb = alpha.x*delta.y-alpha.y*delta.x+r10.y*r30.x-r10.x*r30.y;
    float aa = r10.y*delta.x-r10.x*delta.y;
    float D = bb*bb-4*aa*cc;

    if(D>=0){
        float x1 = (-bb - sqrt(D))/2/aa;
        float x2 = (-bb + sqrt(D))/2/aa;
        float u = 0;
        if(x1>=0 && x1 <=1) u = x1;
        else u = x2;
        vec2 l1 = r0 + u * (r1-r0);
        vec2 l2 = r3 + u * (r2-r3);
        vec2 rp = p0 - l1;
        float v = length(rp)/length(l2-l1)*(dot(normalize(rp),normalize(l2-l1)));

        transf_pos = vec2(u,v);
        color = texture( layerA, transf_pos.xy);
        if( u < 0 || u > 1 ) color = vec4(0);
        if( v < 0 || v > 1 ) color = vec4(0);
    }else return vec4(0);
    } // end of mode 1
    return  color;
}



// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
#ifndef mode_invert_components_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_invert_components_filter(){
#else
vec4 ffilter(){
#endif

    vec4 color = texture( layerA, v2QuadCoords.xy);
    vec4 inversion;
    if(gui_image_type == 3){
            inversion = vec4(gui_inverted_components.r,gui_inverted_components.r,gui_inverted_components.r,0);
    }else{
            inversion = vec4(gui_inverted_components,0);
    }
    vec4 ocolor = inversion - color * inversion;
    ocolor += color*(1-inversion);
    return ocolor;
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
uniform float gui_ao_cancellation;
uniform int gui_remove_shading;

#ifndef mode_ao_cancellation_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_ao_cancellation_filter(){
#else
vec4 ffilter(){
#endif

    float remove_sh = gui_remove_shading/100.0; // normalization to 1
    vec4 colorA   = texture( layerA, v2QuadCoords.xy); // color
    vec4 colorB   = texture( layerB, v2QuadCoords.xy); // ao
    vec4 refColor = texture( layerC, v2QuadCoords.xy); // refcolor

    colorA = mix(refColor,colorA,remove_sh);

    return (colorA + (1-colorB)*gui_ao_cancellation)/(1 + (1-colorB.a)*gui_ao_cancellation);
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------

uniform vec3 average_color;
uniform float gui_remove_shading_lf_blending;

#ifndef mode_remove_low_freq_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_remove_low_freq_filter(){
#else
vec4 ffilter(){
#endif

    vec4 colorA   = texture( layerA, v2QuadCoords.xy);
    vec4 blured   = texture( layerB, v2QuadCoords.xy);
    vec4 finalColor = colorA + (vec4(average_color,1) - blured);

    return mix(colorA,finalColor,gui_remove_shading_lf_blending);
 }

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
vec4 overlay_filter(vec4 colorA, vec4 colorB){
	return colorA * ( colorA + 2 * colorB *( 1 - colorA ) );
}
#ifndef mode_overlay_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_overlay_filter(){
#else
vec4 ffilter(){
#endif

    vec4 colorA = texture( layerA, v2QuadCoords.xy);
    vec4 colorB = texture( layerB, v2QuadCoords.xy);
    return overlay_filter(colorA,colorB);
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
#ifndef mode_invert_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_invert_filter(){
#else
vec4 ffilter(){
#endif

    vec4 color = texture( layerA, v2QuadCoords.xy);
    vec4 icolor = 1 - color;
    icolor.a = 1;
    return icolor;
 }
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------

float gaussian(vec2 pos,float w){
	return exp( (-(pos.x)*(pos.x) - (pos.y)*(pos.y))/(w*w+1.0) );
}

vec4 gauss_filter(sampler2D layer,float w, int radius, float depth){
    vec4 color = vec4(0);
    float totalw = 0;
    for(int i = -radius ; i <= radius ; i++){
            for(int j = -radius ; j <= radius ; j++){
                            float cw = gaussian(vec2(i,j),w);
                            totalw += cw;
                            color   += texture(layer,v2QuadCoords.xy+depth*vec2(i,j)*dxy)*cw;
            }
    }
    return color/totalw;
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
#ifndef mode_gauss_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_gauss_filter(){
#else
vec4 ffilter(){
#endif

    float w    = gui_gauss_w;
    int radius = gui_gauss_radius;

    if(gui_gauss_mask == 0){ // standard processing without masking
        if(gauss_mode == 0){
                return gauss_filter(layerA,w,radius,gui_depth);
        }else if(gauss_mode == 1){
                return gauss_filter_h(layerA,w,radius,gui_depth);
        }else if(gauss_mode == 2){
                return gauss_filter_v(layerA,w,radius,gui_depth);
        }
    }else{ // when using masking texture == 1
        // original color
        vec4  ocolor = texture(layerA,v2QuadCoords.xy);
        // mask texture
        float mask   = abs(float(gui_gauss_invert_mask) - clamp(texture(layerB,v2QuadCoords.xy).r,0,1));
        vec4 scolor  = texture(layerC,v2QuadCoords.xy); // blured image

        // draw mask on image
        if(gui_gauss_show_mask == true){
            vec2 grid = 2*abs(fract(v2QuadCoords.xy*10)-0.5);
            grid      = pow(grid,vec2(16));
            scolor    = mix(vec4(mask),vec4(0.4,0.8,0.2,0),min(grid.x+grid.y,1.0));
        }else scolor  = mix(ocolor,scolor,mask*gui_gauss_blending);
        return scolor;
    } // end of if else masking
}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------




float sobel_edge_detection(){
    // Macro function that updates the sobel matrix values for a specific offset.
    #define UPDATE_SOBEL_VALUES(x, y, sobelMat) \
        (sobelMat)[0] += textureOffset(layerA,v2QuadCoords.st, ivec2( (x), (y) ) ) * sobel_kernel[ (x) + 1 ][ (y) + 1 ];   \
        (sobelMat)[1] += textureOffset(layerA,v2QuadCoords.st, ivec2( (x), (y) ) ) * sobel_kernel[ (y) + 1 ][ (x) + 1 ];

    mat2x4 sobel = mat2x4(0);

    // Unrolled 2D loop over offset values
    // Using unrolled loop + macro function, because the offset parameter of the textureOffset function
    //  must be a constant or literal value.
    UPDATE_SOBEL_VALUES(-1, -1, sobel)
    UPDATE_SOBEL_VALUES(-1,  0, sobel)
    UPDATE_SOBEL_VALUES(-1, +1, sobel)

    UPDATE_SOBEL_VALUES( 0, -1, sobel)
    UPDATE_SOBEL_VALUES( 0,  0, sobel)
    UPDATE_SOBEL_VALUES( 0, +1, sobel)

    UPDATE_SOBEL_VALUES(+1, -1, sobel)
    UPDATE_SOBEL_VALUES(+1,  0, sobel)
    UPDATE_SOBEL_VALUES(+1, +1, sobel)

    return length(sobel[0]) + length(sobel[1]);

    #undef UPDATE_SOBEL_VALUES
}



uniform float make_seamless_radius;
uniform int   gui_seamless_mode;        // 1 - simple , 2 - mirror
uniform int   gui_seamless_mirror_type; // 0 - xy, 1 - x , 2 - y mirror
// atoms positions used in random mode
const float atoms_x[9] = float[](0.0, 0.0, 0.0, 0.5, 0.5 , 0.5 ,1.0,1.0,1.0);
const float atoms_y[9] = float[](0.0, 0.5, 1.0, 0.0, 0.5 , 1.0 ,0.0,0.5,1.0);
uniform mat3 gui_seamless_random_angles;
uniform float gui_seamless_random_phase;
uniform float gui_seamless_random_inner_radius;
uniform float gui_seamless_random_outer_radius;

uniform float gui_seamless_contrast_strenght;
uniform float gui_seamless_contrast_power;

#ifndef mode_seamless_linear_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_seamless_linear_filter(){
#else
vec4 ffilter(){
#endif

    vec2 tc   = v2QuadCoords.st;
    float x   = tc.x;
    float y   = tc.y;
    float rad = make_seamless_radius/2;
    float pwr = gui_seamless_contrast_power*2+0.001;
    float cst = (gui_seamless_contrast_strenght)*20;
    // simple seamless - GIMP like behaviour
    if(gui_seamless_mode == 0){
        vec2 offset = -sign(x-0.5)*vec2(0.5,0);
        vec4 colorA = texture( layerA, tc);
        vec4 colorB = texture( layerA, vec2(tc.x,tc.y) + offset);//*vec4(1,1,0.4,1);
        vec4 hA = texture( layerB, tc);
        vec4 hB = texture( layerB, vec2(tc.x,tc.y) + offset);
        float omega     =  1 - smoothstep(0.0,rad,tc.x) + smoothstep(1.0-rad,1.0,tc.x);
        float bomega    = clamp(omega + omega*pow(length(hA.xyz-hB.xyz)/sqrt(2.0)*cst,pwr),0,1);
        return mix(colorA,colorB,bomega);
    }else{
        vec2 offset = -sign(y-0.5)*vec2(0.0,0.5);
        vec4 colorA = texture( layerA, tc);
        vec4 colorB = texture( layerA, vec2(tc.x,tc.y) + offset);//*vec4(1,1,0.4,1);
        vec4 hA = texture( layerB, tc);
        vec4 hB = texture( layerB, vec2(tc.x,tc.y) + offset);
        float omega     =  1 - smoothstep(0.0,rad,tc.y) + smoothstep(1.0-rad,1.0,tc.y);
        float bomega    = clamp(omega + omega*pow(length(hA.xyz-hB.xyz)/sqrt(2.0)*cst,pwr),0,1);
        return mix(colorA,colorB,bomega);
    }

}


#ifndef mode_seamless_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_seamless_filter(){
#else
vec4 ffilter(){
#endif
    vec2 tc = v2QuadCoords.st;
    float x = tc.x;
    float y = tc.y;

    float pwr = gui_seamless_contrast_power*2+0.001;
    float cst = (gui_seamless_contrast_strenght);

    if(gui_seamless_mode == 2){
        // XY - mirror image
        if(gui_seamless_mirror_type == 0){
            vec4 color = texture( layerA, 2*abs(tc - 0.5));
            if(gui_image_type == 1){
                vec4 nvec = 2*(color)-1;
                float sx = sign(x-0.5);
                float sy = sign(y-0.5);
                color = 0.5*vec4(sx*nvec.x,sy*nvec.y,nvec.z,nvec.w)+0.5;
            }
            return color;
        // X - mirror image
        }else if(gui_seamless_mirror_type == 1){
            vec4 color = texture( layerA, vec2(2*abs(tc.x - 0.5),tc.y));
            if(gui_image_type == 1){
                vec4 nvec = 2*(color)-1;
                float sx = sign(x-0.5);
                float sy = sign(y-0.5);
                color = 0.5*vec4(sx*nvec.x,nvec.y,nvec.z,nvec.w)+0.5;
            }
            return color;
        // Y - mirror image
        }else{
            vec4 color = texture( layerA, vec2(tc.x,2*abs(tc.y - 0.5)));
            if(gui_image_type == 1){
                vec4 nvec = 2*(color)-1;
                float sx = sign(x-0.5);
                float sy = sign(y-0.5);
                color = 0.5*vec4(nvec.x,sy*nvec.y,nvec.z,nvec.w)+0.5;
            }
            return color;

        }

    // random mode
    }else {

        vec4 color         = vec4(0.0);
        //color = texture(layerA,tc);
        highp float weight = 0;
        //vec4 hA = texture( layerB, tc);
        // loop over all atoms
        float total_diff = 0;
        for(int i = 0 ; i < 9 ; i++){
            highp int ii = (i/3)%2;
            highp int jj = i%2;
            highp float angle = gui_seamless_random_angles[ii][jj] + gui_seamless_random_phase;

            // creating the rotation matrix
            highp mat2 rot_mat = mat2(cos(angle),sin(angle),-sin(angle),cos(angle));

            vec2 atom_pos        = vec2(atoms_x[i],atoms_y[i]);
            highp float dist     = distance(tc,atom_pos);
            highp float alpha    = 1-smoothstep(gui_seamless_random_inner_radius,gui_seamless_random_outer_radius,dist);

            //vec4 hB = texture( layerB, rot_mat*((2.5*cst+0.0)*(tc - atom_pos))+0.5);
            float diff  = 0;//100*cst*pow(cst*length(hA.xyz-hB.xyz)/sqrt(2.0),pwr*25+0.0001);
            //hA = hB;
            total_diff += diff;
            alpha                = clamp(alpha + (alpha) * diff,0,1);
            weight              += alpha;
            vec4 ncolor = texture( layerA, (rot_mat*((2.5*cst+0.0)*(tc - atom_pos))+0.5));

            color               += ncolor* alpha;
        }

        float rr = 0.37 - gui_seamless_random_outer_radius;
        if(rr> 0){

        // filling other points
         float x_pos[4] = float[](0.25,0.25,0.75,0.75);
         float y_pos[4] = float[](0.25,0.75,0.25,0.75);
         float rands[4] = float[](gui_seamless_random_angles[0][2],
                                       gui_seamless_random_angles[1][2],
                                       gui_seamless_random_angles[2][2],
                                       gui_seamless_random_angles[2][1]);

        for(int i = 0 ; i < 4 ; i++){
            float angle = rands[i] + gui_seamless_random_phase;

            // creating the rotation matrix
            mat2 rot_mat = mat2(cos(angle),sin(angle),-sin(angle),cos(angle));

            vec2 atom_pos        = vec2(x_pos[i],y_pos[i]);
            float dist     = distance(tc,atom_pos);

            float alpha    = 1-smoothstep(0.0,0.15,dist);
            alpha                = clamp(alpha,0,1);
            weight              += alpha;
            vec4 ncolor = texture( layerA, (rot_mat*(2.5*cst*(angle*0+tc - atom_pos))+0.5));
            color += ncolor* alpha;
        }
        }// end of if rr
        //return color;
        return color/weight;
    }

 }

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------

uniform int   gui_specular_radius;
uniform float gui_specular_w1;
uniform float gui_specular_w2;
uniform float gui_specular_contrast;
uniform float gui_specular_amplifier;
uniform float gui_specular_brightness;

vec4 dgaussian_filter(sampler2D layer, float r1, float r2, int radius, float depth ){
    vec4 colorA = vec4(0);
    vec4 colorB = vec4(0);

    colorA = gauss_filter(layer,r1,radius,depth);
    colorB = gauss_filter(layer,r2,radius,depth);
    return colorA - colorB;
}

#ifndef mode_dgaussians_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_dgaussians_filter(){
#else
vec4 ffilter(){
#endif


    vec4 c1 =texture(layerA,v2QuadCoords.xy);
    vec4 c2 =texture(layerB,v2QuadCoords.xy);
    vec4 dc = c1-c2;



    if(gui_mode_dgaussian ==  0){
            return dc;
    }else{    
        if(gui_specular_amplifier > 0){
                return clamp(3*gui_specular_amplifier*dc,vec4(0),vec4(1));
        }else{
                return clamp(1+dc*gui_specular_amplifier*3,vec4(0),vec4(1));
        }
    }
}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
vec4 contrast_filter(vec4 color,float contrast){
    float c = 1.01 * (contrast+1.0)/(1.01-contrast);
    return c * (color-0.5) + 0.5;
}

#ifndef mode_constrast_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_constrast_filter(){
#else
vec4 ffilter(){
#endif

    vec4 color = texture( layerA, v2QuadCoords.xy);
    vec4 icolor = contrast_filter(color,gui_specular_contrast);
    icolor = clamp(icolor,vec4(0),vec4(1));
    return clamp(icolor+gui_specular_brightness,vec4(0),vec4(1));
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
uniform vec3 gui_gray_scale_preset;

#ifndef mode_gray_scale_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_gray_scale_filter(){
#else
vec4 ffilter(){
#endif

	
    vec4 color = texture( layerA, v2QuadCoords.xy);
//    vec3 clevel = color.rgb;
    color = vec4(1)*(color.r * gui_gray_scale_preset.r + color.g * gui_gray_scale_preset.g + color.b * gui_gray_scale_preset.b);
//    float cdist = length(clevel - vec3(1,0,0));
//    return clamp(vec4(cdist),vec4(0),vec4(1));
    return clamp(color,vec4(0),vec4(1));
}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
#ifndef mode_normalize_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_normalize_filter(){
#else
vec4 ffilter(){
#endif

    vec4 color = texture( layerA, v2QuadCoords.xy);
    color.rgb =  ( color.rgb - min_color )/(max_color-min_color) ;
    color.a = 1;
    return color;
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
#ifndef mode_small_details_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_small_details_filter(){
#else
vec4 ffilter(){
#endif


    vec4 colorA   = texture( layerA, v2QuadCoords.xy);
    vec4 colorB   =-texture( layerB, v2QuadCoords.xy);
    vec4 colorC   = 1-20*gui_small_details*colorB;
    return clamp(overlay_filter(colorA,colorC*0.5),vec4(0),vec4(1));

}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
#ifndef mode_medium_details_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_medium_details_filter(){
#else
vec4 ffilter(){
#endif


    vec4 colorA      = texture( layerA, v2QuadCoords.xy); // oryginal texture (not blured)
    vec4 bluredBig   = texture( layerB, v2QuadCoords.xy);
    vec4 bluredSmall = gauss_filter(layerA,gui_gauss_w,gui_gauss_radius,gui_depth/2);


    vec4 dog = bluredSmall - bluredBig;

    //return dog;
    vec4 colorC   = 1+20*gui_small_details*dog;
    return clamp(overlay_filter(colorA,colorC*0.5),vec4(0),vec4(1));
}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
#ifndef mode_sharpen_blur_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_sharpen_blur(){
#else
vec4 ffilter(){
#endif

    vec4 gcolor;
    int radius 	  = abs(gui_sharpen_blur);
    float w 		  = radius;
    if(gauss_mode == 0){
            gcolor = gauss_filter(layerA,w,radius,1.0);
    }else if(gauss_mode==1){
            gcolor = gauss_filter_h(layerA,w,radius,1.0);
    }else if(gauss_mode==2){
            gcolor = gauss_filter_v(layerA,w,radius,1.0);
    }
    if(gui_sharpen_blur > 0){
            vec4  color  = texture(layerA, v2QuadCoords.st);
            color.xyz   +=  color.xyz - gcolor.xyz;
            color = clamp(color,vec4(0),vec4(1));
            return color;
    }else{
            return gcolor;
    }
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
#ifndef mode_normals_step_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_normals_step_filter(){
#else
vec4 ffilter(){
#endif

	
        vec4 color = 2*(texture( layerA, v2QuadCoords.xy) - 0.5);

        color.xy *= gui_normals_step;//(1+2*gui_normals_step);

        color.xyz = normalize(color.xyz);

	return color*0.5 +0.5;
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
uniform float gui_normal_mixer_depth;
uniform float gui_normal_mixer_angle;
uniform float gui_normal_mixer_scale;
uniform float gui_normal_mixer_pos_x;
uniform float gui_normal_mixer_pos_y;

#ifndef mode_normal_mixer_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_normal_mixer_filter(){
#else
vec4 ffilter(){
#endif


        vec2 newTexCoords = vec2(v2QuadCoords.xy-0.5) - vec2(gui_normal_mixer_pos_x,gui_normal_mixer_pos_y);
        // rotate
        newTexCoords = vec2(newTexCoords.x*cos(gui_normal_mixer_angle)+
                            newTexCoords.y*sin(gui_normal_mixer_angle),
                           -newTexCoords.x*sin(gui_normal_mixer_angle)+
                            newTexCoords.y*cos(gui_normal_mixer_angle));
        // scale coords
        newTexCoords = newTexCoords*gui_normal_mixer_scale;

        newTexCoords = newTexCoords + 0.5;

        float ndepth = gui_normal_mixer_depth/50;
        vec3 normalA = 2*(texture( layerA, v2QuadCoords.xy).xyz - 0.5);
        vec3 normalB = 2*(texture( layerB, newTexCoords.xy).xyz - 0.5);
        // rotate normals
        normalB = vec3(normalB.x*cos(-gui_normal_mixer_angle)+
                       normalB.y*sin(-gui_normal_mixer_angle),
                      -normalB.x*sin(-gui_normal_mixer_angle)+
                       normalB.y*cos(-gui_normal_mixer_angle),
                       normalB.z);

        vec3 finalNormal = vec3(normalA.xy,normalA.z)+
                           vec3(normalB.xy*ndepth,0);

        vec4 finalColor  = vec4((finalNormal)*0.5+0.5,1);


        return finalColor;
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
#ifndef mode_normal_to_height_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_normal_to_height(){
#else
vec4 ffilter(){
#endif

	float scale   = hn_min_max_scale.z;

	vec3 off  = scale*vec3(vec2(-1,1)*dxy,0);
	vec2 tex_coord = v2QuadCoords.st;

        float hxp  = texture(layerA,tex_coord + off.yz).x;
        float hxm  = texture(layerA,tex_coord + off.xz).x;
        float hyp  = texture(layerA,tex_coord + off.zy).x;
        float hym  = texture(layerA,tex_coord + off.zx).x;

        float nxp  = 2*(texture(layerB,tex_coord + off.yz ).x-0.5);
        float nxm  = 2*(texture(layerB,tex_coord + off.xz ).x-0.5);

        float nyp  = 2*(texture(layerB,tex_coord + off.zy ).y-0.5);
        float nym  = 2*(texture(layerB,tex_coord + off.zx ).y-0.5);

        float h = (nxp-nxm+nyp-nym)/8.0*scale + (hxp + hxm + hyp + hym)/4.0;
        //h =   h / (1.0 + abs(nxp-nxm+nyp-nym)/8.0*scale);

        return vec4(h);
	
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------

#ifndef mode_height_to_normal_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_height_to_normal(){
#else
vec4 ffilter(){
#endif


    const vec2 size = vec2(1.0,0.0);
    const ivec3 off = ivec3(-1,0,1);
    vec2 tex_coord =  v2QuadCoords.st;

    vec4 hc   = texture(layerA, tex_coord);
    highp float s11 = hc.x;
    highp float s21 = textureOffset(layerA, tex_coord, off.zy).x;
    highp float s12 = textureOffset(layerA, tex_coord, off.yz).x;
    highp vec3 va   = normalize(vec3(size.xy,gui_hn_conversion_depth*(s21-s11)));
    highp vec3 vb   = normalize(vec3(size.yx,gui_hn_conversion_depth*(s12-s11)));
    highp vec3 bump = normalize( cross(va,vb) );
    return vec4(clamp(bump*0.5 + 0.5,vec3(0),vec3(1)),1);
}



// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
#ifndef mode_sobel_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_sobel_filter(){
#else
vec4 ffilter(){
#endif

    float sobel_x = 0;
    float sobel_y = 0;

    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 3 ; j++){
            sobel_x   += texture(layerA,v2QuadCoords.xy+vec2(i-1,j-1)*dxy).r*sobel_kernel[i][j];
            sobel_y   -= texture(layerA,v2QuadCoords.xy+vec2(i-1,j-1)*dxy).r*sobel_kernel[j][i];
        }
    }
    vec3 n = normalize(vec3(gui_basemap_amp*vec2(sobel_y,sobel_x),1));
    vec4 ocolor= vec4(0);
    ocolor.rgb = clamp(n*0.5+0.5,vec3(0),vec3(1));
    ocolor.rgb = vec3(1-ocolor.r,ocolor.gb);
    return ocolor;
}


// ----------------------------------------------------------------
// angle correction
// ----------------------------------------------------------------
uniform float base_map_angle_correction;
uniform float base_map_angle_weight;
#ifndef mode_normal_angle_correction_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_normal_angle_correction_filter(){
#else
vec4 ffilter(){
#endif
    vec3 normal   = normalize(texture(layerA,v2QuadCoords.xy).rgb-0.5);
    vec2 anglevec = vec2(cos(base_map_angle_correction),sin(base_map_angle_correction));
    float ndota   = dot(anglevec,normal.st)*base_map_angle_weight*2.0;
    normal = normalize(vec3(normal.x,normal.y,abs(normal.z + ndota)));
    return vec4(normal+0.5,1.0);
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------

#ifndef mode_normal_expansion_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_normal_expansion_filter(){
#else
vec4 ffilter(){
#endif

    vec3 filt  = vec3(0);
    float wtotal = 0.0;
    int radius   = gui_filter_radius;

    // normal expansion
    if(gui_combine_normals == 0){

    for(int i = -radius ; i <= radius ; i++){
    for(int j = -radius ; j <= radius ; j++){
        vec2 coords = vec2(v2QuadCoords.xy+vec2(i,j)*dxy);
        vec3 normal = normalize(2*texture(layerA,coords).xyz-1);

        float w = mix(length(normal.xy),
                      1/(20*gaussian(vec2(i,j),gui_filter_radius)*length(normal.xy)+1),
                      gui_normal_flatting+0.001);
        wtotal  += w;
        filt  += normal*w;
    }}
    filt /= (wtotal);//normalization

    return vec4(0.5*normalize(filt)+0.5,1);

    }else{// blending and slope-based mixing

    vec3 n      = normalize(texture(layerA,v2QuadCoords.xy).xyz*2-1);//get normal
    float slope = (1.0/(exp(+10*gui_mix_normals* length(n.xy))+1)); // slope based blending param

    vec4 a = texture(layerA,v2QuadCoords.xy);//processed normal image
    vec4 b = texture(layerB,v2QuadCoords.xy);//normal image obtained from sobel filter
    return  mix(mix(a,b,slope),a,gui_blend_normals);

    }
	
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
// normal mipmaps weights
uniform float gui_base_map_w0;
uniform float gui_base_map_w1;
uniform float gui_base_map_w2;
uniform float gui_base_map_w3;
#ifndef mode_mix_normal_levels_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_mix_normal_levels_filter(){
#else
vec4 ffilter(){
#endif

    vec3 normalA     = normalize(texture(layerA,v2QuadCoords.xy).rgb - 0.5);
    vec3 normalB     = normalize(texture(layerB,v2QuadCoords.xy).rgb - 0.5);
    vec3 normalC     = normalize(texture(layerC,v2QuadCoords.xy).rgb - 0.5);
    vec3 normalD     = normalize(texture(layerD,v2QuadCoords.xy).rgb - 0.5);

    vec3 finalNormal = vec3(normalA.xy*gui_base_map_w0,normalA.z) +
                       vec3(normalB.xy*gui_base_map_w1,normalB.z) +
                       vec3(normalC.xy*gui_base_map_w2,normalC.z) +
                       vec3(normalD.xy*gui_base_map_w3,normalD.z);

    vec4 finalColor  = vec4(finalNormal+0.5,1);

    return finalColor;
}



// ----------------------------------------------------------------
//
// ----------------------------------------------------------------

float g_intensity 	          = gui_ssao_intensity; // intensywnosc efektu
float g_scale 			  = 5*gui_ssao_depth; // dlugosc promienia prubkujacego
float g_bias 			  = 0.05*gui_ssao_bias; //  controls the width of the occlusion cone considered by the occludee.
// Pobieranie pozycji teksla w ukladzie oka
vec3 getPosition(vec2 uv){
        return vec3(uv,-g_bias*texture(layerA,uv).r);
}
// Pobieranie normalnej
vec3 getNormal(vec2 uv){
        return normalize(texture(layerB, uv).xyz-0.5);
}

float doAmbientOcclusion(in vec2 tcoord,in vec2 uv, in vec3 p, in vec3 cnorm){
	vec3 diff = getPosition(tcoord + uv) - p;
	vec3 v = normalize(diff);
	return max(0.0,dot(cnorm,v))*g_intensity;
}


#ifndef mode_occlusion_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_occlusion_filter(){
#else
vec4 ffilter(){
#endif
	vec3 p 		= getPosition(v2QuadCoords.xy); // pozycja w ukladzie oka
	vec3 n 		= getNormal(v2QuadCoords.xy); // normalna
	float ao      = 0.0f;
	for(int i = -gui_ssao_no_iters ; i <=  gui_ssao_no_iters ; i++){
		for(int j = -gui_ssao_no_iters ; j <=  gui_ssao_no_iters ; j++){
				ao += doAmbientOcclusion(v2QuadCoords,vec2(i,j)*dxy*g_scale, p, n);
	}}
	ao /= (2*gui_ssao_no_iters+1.0)*(2*gui_ssao_no_iters+1.0);	

        return vec4(clamp(1-ao,0,1));        
}
#ifndef mode_combine_normal_height_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_combine_normal_height_filter(){
#else
vec4 ffilter(){
#endif
        vec4 colorA   = texture( layerA, v2QuadCoords.xy);
        vec4 colorB   = texture( layerB, v2QuadCoords.xy);
	return vec4(colorA.rgb,colorB.r);
}

// ----------------------------------------------------------------
//
// Process height image in order to make image flatter, etc
// ----------------------------------------------------------------
uniform float gui_height_proc_min_value;
uniform float gui_height_proc_max_value;
uniform int gui_height_proc_ave_radius;
uniform float gui_height_proc_offset_value;
uniform bool gui_height_proc_normalization;
vec4 height_clamp(vec4 inputc, vec4 value,float vmin,float vmax){
    vec4 dmax_ave = vec4(vmax) - value;
    if(dmax_ave.r < 0) inputc.r += dmax_ave.r;
    if(dmax_ave.g < 0) inputc.g += dmax_ave.g;
    if(dmax_ave.b < 0) inputc.b += dmax_ave.b;
    vec4 dmin_ave = vec4(vmin) - value;
    if(dmin_ave.r > 0) inputc.r   += dmin_ave.r;
    if(dmin_ave.g > 0) inputc.g   += dmin_ave.g;
    if(dmin_ave.b > 0) inputc.b   += dmin_ave.b;
    return inputc;
}

vec4 height_clamp2(vec4 inputc, float weight,float vmin,float vmax){
    vec4 dmax_ave = weight*(inputc  - vec4(vmax));
    vec4 outputValue = inputc;
    if(inputc.r > vmax) outputValue.r = vmax + dmax_ave.r;
    if(inputc.g > vmax) outputValue.g = vmax + dmax_ave.g;
    if(inputc.b > vmax) outputValue.b = vmax + dmax_ave.b;

    vec4 dmin_ave = weight*(inputc  - vec4(vmin));
    if(inputc.r < vmin) outputValue.r = vmin + dmin_ave.r;
    if(inputc.g < vmin) outputValue.g = vmin + dmin_ave.g;
    if(inputc.b < vmin) outputValue.b = vmin + dmin_ave.b;
    return outputValue;
}
#ifndef mode_height_processing_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_height_processing_filter(){
#else
vec4 ffilter(){
#endif

    int radius      = gui_height_proc_ave_radius/5+1;
    float w         = gui_height_proc_ave_radius/50.0; // "details" slider in gui
    vec4 height     = texture( layerA, v2QuadCoords.xy);

    vec4 ave_color = vec4(0.0);
    int no_samples  = 0;

    for(int x  = -(radius-1) ; x <= radius-1 ; x++){
        for(int y  = -(radius-1) ; y <= radius-1 ; y++){
            ave_color += texture( layerA, v2QuadCoords.xy + vec2(x,y)*dxy);
            no_samples++;
    }}
    ave_color /= no_samples;

    height = height_clamp(height,ave_color,gui_height_proc_min_value,gui_height_proc_max_value);
    vec4 hmin = height_clamp(vec4(0.0),vec4(0.0),gui_height_proc_min_value,gui_height_proc_max_value);
    vec4 hmax = height_clamp(vec4(1.0),vec4(1.0),gui_height_proc_min_value,gui_height_proc_max_value);
    if(gui_height_proc_normalization){
        return clamp(vec4(height-hmin)/(hmax-hmin) + vec4(gui_height_proc_offset_value),vec4(0),vec4(1));
    }else{
        return clamp(vec4(height-hmin) + vec4(gui_height_proc_offset_value),vec4(0),vec4(1));
    }

}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------


uniform float gui_roughness_depth;
uniform float gui_roughness_treshold;
uniform float gui_roughness_amplifier;

#ifndef mode_roughness_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_roughness_filter(){
#else
vec4 ffilter(){
#endif


    float depth     = gui_roughness_depth ;
    float treshold  = gui_roughness_treshold/20.0;
    float amplifier = gui_roughness_amplifier*50;
    float ave       = texture( layerB, v2QuadCoords.xy ).r;


    float curvature = 0;
    for(int x  = -10 ; x <= 10 ; x++){
        for(int y  = -10 ; y <= 10 ; y++){
          float val = texture( layerA, v2QuadCoords.xy + depth * vec2(x,y) * dxy ).r;
          curvature += pow(val - ave,2.0) ;
    }}
    curvature = sqrt(curvature/100.0);
    float cc = curvature;
    float dc =  clamp(clamp(  curvature - abs(treshold) ,0.0,1.0) * amplifier,0,1);
    if(treshold < 0) dc = 1 - dc;

    return  vec4(dc) ;
}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
uniform vec3 gui_roughness_picked_color;
uniform int  gui_roughness_color_method;
uniform bool gui_roughness_invert_mask;
uniform float gui_roughness_color_offset;
uniform float gui_roughness_color_global_offset;
uniform float gui_roughness_color_amplifier;
#ifndef mode_roughness_color_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_roughness_color_filter(){
#else
vec4 ffilter(){
#endif

    // differente approaches to measure difference in colors
    float angle;
    float amp = (1+5*gui_roughness_color_amplifier);
    float offset = gui_roughness_color_offset*2;

    if(gui_roughness_color_method == 0){ // angle based A
        // treat colors like normals
        vec3 colorA     = normalize(texture( layerA, v2QuadCoords.xy).rgb-0.5);
        vec3 maskColor  = normalize(gui_roughness_picked_color-0.5); // normalized value of picked color
        angle           = clamp(dot(colorA,maskColor)*amp+offset,0.0,1.0);

    }else if(gui_roughness_color_method == 1){ // angle based B
        vec3 colorA     = normalize(texture( layerA, v2QuadCoords.xy).rgb);
        vec3 maskColor  = normalize(gui_roughness_picked_color); // normalized value of picked color
        angle           = clamp(dot(colorA,maskColor)*amp+offset,0.0,1.0);

    }else if(gui_roughness_color_method == 2){ // distance based
        vec3 colorA     = (texture( layerA, v2QuadCoords.xy).rgb);
        vec3 maskColor  = (gui_roughness_picked_color);
        angle           = clamp((1-distance(colorA,maskColor)/sqrt(3.0))*amp + offset,0.0,1.0);

    }
    float value = abs(float(gui_roughness_invert_mask)-angle)+gui_roughness_color_global_offset;
    return vec4(clamp(value,0,1)) ;
}

// ----------------------------------------------------------------
//  Grunge
// ----------------------------------------------------------------
uniform float gui_grunge_overall_weight;
uniform int gui_grunge_blending_mode;

// based on GIMP: http://docs.gimp.org/en/gimp-concepts-layer-modes.html
#define GRUNGE_NORMAL       0
#define GRUNGE_MULTIPLY     1
#define GRUNGE_ADD          2
#define GRUNGE_SUBTRACT     3
#define GRUNGE_DIFFERENCE   4
#define GRUNGE_DIVIDE       5
#define GRUNGE_SCREEN       6
#define GRUNGE_OVERLAY      7
#define GRUNGE_DODGE        8
#define GRUNGE_BURN         9
#define GRUNGE_DARKEN_ONLY  10
#define GRUNGE_LIGHTEN_ONLY 11


#ifndef mode_grunge_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_grunge_filter(){
#else
vec4 ffilter(){
#endif

    vec4 I  = texture( layerA, v2QuadCoords.xy);
    vec4 M  = texture( layerB, v2QuadCoords.xy);
    float w = gui_grunge_overall_weight;
    vec4 color = I;

    if(gui_grunge_blending_mode == GRUNGE_NORMAL){
       color = M;
    }else if(gui_grunge_blending_mode == GRUNGE_MULTIPLY){
       color = I*M;
    }else if(gui_grunge_blending_mode == GRUNGE_ADD){
       color = clamp(I+M,vec4(0.0),vec4(1.0));
    }else if(gui_grunge_blending_mode == GRUNGE_SUBTRACT){
        color = clamp(I-M,vec4(0.0),vec4(1.0));
        float gray = dot(color.rgb,vec3(1));
        color = gray * I;
    }else if(gui_grunge_blending_mode == GRUNGE_DIFFERENCE){
        color = clamp((I-M),vec4(0.0),vec4(1.0));
        float gray = dot(color.rgb,vec3(1));
        color = gray * I;
    }else if(gui_grunge_blending_mode == GRUNGE_DIVIDE){
        color = clamp(I/(M+0.004),vec4(0.0),vec4(1.0));
    }else if(gui_grunge_blending_mode == GRUNGE_SCREEN){
        vec4 ocolor = 1.0 - (1-I)*(1-M);
        color = clamp(ocolor,vec4(0.0),vec4(1.0));
    }else if(gui_grunge_blending_mode ==  GRUNGE_OVERLAY){
        vec4 ocolor =  I * ( I + 2 * M *( 1 - I ) );
        color = clamp(ocolor,vec4(0.0),vec4(1.0));
    }else if(gui_grunge_blending_mode ==  GRUNGE_DODGE){
        vec4 ocolor =  I / ((1-M)+0.004);
        color = clamp(ocolor,vec4(0.0),vec4(1.0));
    }else if(gui_grunge_blending_mode ==  GRUNGE_BURN){
        vec4 ocolor = 1 - (1-I)/(M+0.004);
        color = clamp(ocolor,vec4(0.0),vec4(1.0));
    }else if(gui_grunge_blending_mode ==  GRUNGE_DARKEN_ONLY){
        vec4 ocolor = min(I,M);
        color = clamp(ocolor,vec4(0.0),vec4(1.0));
    }else if(gui_grunge_blending_mode ==  GRUNGE_LIGHTEN_ONLY){
        vec4 ocolor = max(I,M);
        color = clamp(ocolor,vec4(0.0),vec4(1.0));
    }


    return mix(I,color,w);
}


// ----------------------------------------------------------------
//  Grunge randomization
// ----------------------------------------------------------------
uniform float gui_grunge_radius;
uniform int gui_grunge_translations;
uniform bool gui_grunge_brandomize;
#ifndef mode_grunge_randomization_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_grunge_randomization_filter(){
#else
vec4 ffilter(){
#endif

    float rand_scale = gui_grunge_radius/25.0;

    if(!gui_grunge_brandomize){
           return texture( layerA, v2QuadCoords.xy*rand_scale);
    }

    vec4 color         = vec4(0.0);
    float weight = 0;
    // loop over all atoms
    vec2 tc = v2QuadCoords.st;
    float x = tc.x;
    float y = tc.y;

    float translate = gui_grunge_translations;

    for(int i = 0 ; i < 9 ; i++){
        int ii = (i/3)%2;
        int jj = i%2;
        float angle = gui_seamless_random_angles[ii][jj] + gui_seamless_random_phase;

        // creating the rotation matrix
        mat2 rot_mat = mat2(cos(angle),sin(angle),-sin(angle),cos(angle));

        vec2 atom_pos        = vec2(atoms_x[i],atoms_y[i]);
        float dist     = distance(tc,atom_pos);
        float alpha    = 1-smoothstep(0.0,0.37,dist);

        alpha                = clamp(alpha,0,1);

        vec4 ncolor = texture( layerA, (rot_mat*(rand_scale*(angle*translate+tc - atom_pos))+0.5));

        color = max(color,ncolor*smoothstep(0.1,0.2,alpha));

    }
     // Filling other points
     float x_pos[4] = float[](0.25,0.25,0.75,0.75);
     float y_pos[4] = float[](0.25,0.75,0.25,0.75);
     float rands[4] = float[](gui_seamless_random_angles[0][2],
                                   gui_seamless_random_angles[1][2],
                                   gui_seamless_random_angles[2][2],
                                   gui_seamless_random_angles[2][1]);

    for(int i = 0 ; i < 4 ; i++){
        float angle = rands[i] + gui_seamless_random_phase;

        // creating the rotation matrix
        mat2 rot_mat = mat2(cos(angle),sin(angle),-sin(angle),cos(angle));

        vec2 atom_pos  = vec2(x_pos[i],y_pos[i]);
        float dist     = distance(tc,atom_pos);
        float alpha    = 1-smoothstep(0.0,0.2,dist);
        alpha          = clamp(alpha,0,1);

        vec4 ncolor = texture( layerA, (rot_mat*(rand_scale*(angle*translate+tc - atom_pos))+0.5));

        color = max(color,ncolor*smoothstep(0.1,0.3,alpha));
    }

    //weight = 1.0;
    return clamp(color,vec4(0.0),vec4(1.0));

}


// ----------------------------------------------------------------
//  Grunge normal warp
// ----------------------------------------------------------------
uniform float grunge_normal_warp;
#ifndef mode_grunge_normal_warp_filter_330
#ifndef USE_OPENGL_330
subroutine(filterModeType)
#endif
vec4 mode_grunge_normal_warp_filter(){
#else
vec4 ffilter(){
#endif
    vec2 normal = texture( layerB, v2QuadCoords.xy).st - 0.5;
    vec4 color  = texture( layerA, v2QuadCoords.xy + normal*grunge_normal_warp*0.05);

    return color;
}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------


out vec4 FragColor;
uniform int material_id;

void main() {   


    vec3 materialColor = texture( materialTexture, v2QuadCoords.xy).rgb;

    int materialIndex = int(255*255*255*materialColor.r)+int(255*255*materialColor.g)+int(255*materialColor.b);

    if(material_id >= 0){// if current ID is different than -1
        if( materialIndex == material_id ){ // compare colors and process only the mask region
            #ifndef USE_OPENGL_330
            FragColor   = filterMode();
            #else
            FragColor   = ffilter();
            #endif
        }else{
            discard;
        }
    }else if(material_id == -1){//draw just last image
        FragColor = texture( layerA, v2QuadCoords.xy);
    }else{// normal processing (materials disabled)
            #ifndef USE_OPENGL_330
            FragColor   = filterMode();
            #else
            FragColor   = ffilter();
            #endif
    }



 }
