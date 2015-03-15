<<<<<<< HEAD:content/filters.frag
#version 400 core


subroutine vec4 filterModeType();
subroutine uniform filterModeType filterMode;

uniform sampler2D layerA; // first layer
uniform sampler2D layerB; // second layer
uniform sampler2D layerC; // third layer

uniform int quad_draw_mode;

uniform int gauss_mode;


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
uniform	float gui_gauss_w;
uniform	int gui_gauss_radius;
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

const mat3 sobel_kernel  = mat3(
                        -1.0, 0.0, +1.0,
                        -2.0, 0.0, +2.0,
                        -1.0, 0.0, +1.0
                                );

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normal_filter(){		
    vec4 c =  texture( layerA, v2QuadCoords.xy);
    if(gui_clear_alpha == 1) c = vec4(c.xyz,1);
    return c;
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

subroutine(filterModeType) vec4 mode_perspective_transform_filter(){
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
subroutine(filterModeType) vec4 mode_invert_components_filter(){
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
vec4 overlay_filter(vec4 colorA, vec4 colorB){
	return colorA * ( colorA + 2 * colorB *( 1 - colorA ) );
}

subroutine(filterModeType) vec4 mode_overlay_filter(){
    vec4 colorA = texture( layerA, v2QuadCoords.xy);
    vec4 colorB = texture( layerB, v2QuadCoords.xy);
    return overlay_filter(colorA,colorB);
}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_invert_filter(){
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

subroutine(filterModeType) vec4 mode_gauss_filter(){
    float w = gui_gauss_w;
    int radius = gui_gauss_radius;
    if(gauss_mode == 0){
            return gauss_filter(layerA,w,radius,gui_depth);
    }else if(gauss_mode == 1){
            return gauss_filter_h(layerA,w,radius,gui_depth);
    }else if(gauss_mode == 2){
            return gauss_filter_v(layerA,w,radius,gui_depth);
    }
}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
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

subroutine(filterModeType) vec4 mode_seamless_filter(){
		
    vec2 tc = v2QuadCoords.st;
    float x = tc.x;
    float y = tc.y;
    // simple seamless - GIMP like behaviour
    if(gui_seamless_mode == 1){

        vec2 r = tc  - 0.502;
        vec2 rdir    = 0.5*vec2(sign(r.x),sign(r.y));
        float rl       = 2*length(r);
        float scale  = 1.0;
        float alpha = min(scale,pow(rl,(1+20*make_seamless_radius)))/scale;

        vec4 colorA = texture( layerA, tc);
        vec4 colorB = texture( layerA, tc + rdir);
        float sq = abs(pow(2*abs(x-0.5),1+50*make_seamless_radius)) + abs(pow(2*abs(y-0.5),1+50*make_seamless_radius));

        return mix(colorA,colorB,alpha*clamp(sq,0,1));

    // mirror mode
    }else if(gui_seamless_mode == 2){
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
        highp float weight = 0;
        // loop over all atoms
        for(int i = 0 ; i < 9 ; i++){
            highp int ii = (i/3)%2;
            highp int jj = i%2;
            highp float angle = gui_seamless_random_angles[ii][jj] + gui_seamless_random_phase;

            // creating the rotation matrix
            highp mat2 rot_mat = mat2(cos(angle),sin(angle),-sin(angle),cos(angle));

            vec2 atom_pos        = vec2(atoms_x[i],atoms_y[i]);
            highp float dist     = distance(tc,atom_pos);
            highp float alpha    = 1-smoothstep(gui_seamless_random_inner_radius,gui_seamless_random_outer_radius,dist);

            weight              += alpha;
            color               += texture( layerA, rot_mat*(tc - atom_pos)+0.5)* alpha;
        }
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


subroutine(filterModeType) vec4 mode_dgaussians_filter(){

    vec4 c1 =texture(layerA,v2QuadCoords.xy);
    vec4 c2 =texture(layerB,v2QuadCoords.xy);
    vec4 dc = c1-c2;
    if(gui_mode_dgaussian ==  0){
            return dc;
    }else{
    //dc = vec4(1)*dot(dc.rgb,vec3(1))/3;
    if(gui_specular_amplifier > 0){
            return 3*gui_specular_amplifier*dc;
    }else{
            return 1+dc*gui_specular_amplifier*3;
    }	}
}

 // ----------------------------------------------------------------
//
// ----------------------------------------------------------------
vec4 contrast_filter(vec4 color,float contrast){
    float c = 1.01 * (contrast+1.0)/(1.01-contrast);
    return c * (color-0.5) + 0.5;
}
subroutine(filterModeType) vec4 mode_constrast_filter(){
    vec4 color = texture( layerA, v2QuadCoords.xy);
    vec4 icolor = contrast_filter(color,gui_specular_contrast);
    icolor = clamp(icolor,vec4(0),vec4(1));
    return clamp(icolor+gui_specular_brightness,vec4(0),vec4(1));
}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
uniform vec3 gui_gray_scale_preset;
subroutine(filterModeType) vec4 mode_gray_scale_filter(){
	
    vec4 color = texture( layerA, v2QuadCoords.xy);
    color = vec4(1)*(color.r * gui_gray_scale_preset.r + color.g * gui_gray_scale_preset.g + color.b * gui_gray_scale_preset.b);
    return clamp(color,vec4(0),vec4(1));
}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normalize_filter(){		
    vec4 color = texture( layerA, v2QuadCoords.xy);
    color.rgb =  ( color.rgb - min_color )/(max_color-min_color) ;
    color.a = 1;
    return color;
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_small_details_filter(){

    vec4 colorA   = texture( layerA, v2QuadCoords.xy);
    vec4 colorB   =-texture( layerB, v2QuadCoords.xy);
    vec4 colorC   = 1-20*gui_small_details*colorB;
    return clamp(overlay_filter(colorA,colorC*0.5),vec4(0),vec4(1));

}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_medium_details_filter(){



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
subroutine(filterModeType) vec4 mode_sharpen_blur(){
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
subroutine(filterModeType) vec4 mode_normals_step_filter(){
	
        vec4 color = 2*(texture( layerA, v2QuadCoords.xy) - 0.5);

        color.xy *= gui_normals_step;//(1+2*gui_normals_step);


        color.xyz = normalize(color.xyz);

	return color*0.5 +0.5;
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normal_to_height(){	

	float scale   = hn_min_max_scale.z;

	vec3 off  = scale*vec3(vec2(-1,1)*dxy,0);
	vec2 tex_coord = v2QuadCoords.st;


	
	float hxp  = texture(layerA,tex_coord + off.yz).x;
	float hxm = texture(layerA,tex_coord + off.xz).x;
	float hyp  = texture(layerA,tex_coord + off.zy).x;
	float hym = texture(layerA,tex_coord + off.zx).x;	
	
	float nxp   = 2*(texture(layerB,tex_coord + off.yz ).x-0.5);
	float nxm  = 2*(texture(layerB,tex_coord + off.xz ).x-0.5);
	
	float nyp   = 2*(texture(layerB,tex_coord + off.zy ).y-0.5);
	float nym  = 2*(texture(layerB,tex_coord + off.zx ).y-0.5);
	
	float h = (nxp-nxm+nyp-nym)/8.0*scale + (hxp + hxm + hyp + hym)/4.0;	


        return vec4(h);
	
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_height_to_normal(){

    const vec2 size = vec2(1.0,0.0);
    const ivec3 off = ivec3(-1,0,1);
    vec2 tex_coord =  v2QuadCoords.st;

    vec4 hc   = texture(layerA, tex_coord);
    float s11 = hc.x;    
    float s21 = textureOffset(layerA, tex_coord, off.zy).x;    
    float s12 = textureOffset(layerA, tex_coord, off.yz).x;
    vec3 va   = normalize(vec3(size.xy,gui_hn_conversion_depth*(s21-s11)));
    vec3 vb   = normalize(vec3(size.yx,gui_hn_conversion_depth*(s12-s11)));
    vec3 bump = normalize( cross(va,vb) );
    return vec4(clamp(bump*0.5 + 0.5,vec3(0),vec3(1)),1);
}



// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_sobel_filter(){
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
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normal_expansion_filter(){
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
subroutine(filterModeType) vec4 mode_smooth_filter(){
	float w     = gui_smooth_radius;
	int radius = int(w)+1;
	return gauss_filter(layerA,w,radius,1.0);
}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------

float g_intensity 	          = gui_ssao_intensity; // intensywnosc efektu
float g_scale 			  = 5*gui_ssao_depth; // dlugosc promienia prubkujacego
float g_bias 			  = 0.05*gui_ssao_bias; //  controls the width of the occlusion cone considered by the occludee.
// Pobieranie pozycji teksla w ukladzie oka
vec3 getPosition(vec2 uv){
	return vec3(uv,-g_bias*texture(layerA,uv).w);//texture(positionFBO,uv).xyz;
}
// Pobieranie normalnej
vec3 getNormal(vec2 uv){
	return normalize(texture(layerA, uv).xyz-0.5);
}

float doAmbientOcclusion(in vec2 tcoord,in vec2 uv, in vec3 p, in vec3 cnorm){
	vec3 diff = getPosition(tcoord + uv) - p;
	vec3 v = normalize(diff);
	return max(0.0,dot(cnorm,v))*g_intensity;
}
subroutine(filterModeType) vec4 mode_occlusion_filter(){		
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

subroutine(filterModeType) vec4 mode_combine_normal_height_filter(){
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

subroutine(filterModeType) vec4 mode_height_processing_filter(){
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
    return vec4(height-hmin)/(hmax-hmin);
}


out vec4 FragColor;
void main() {   
	FragColor   = filterMode();
 }
=======
#version 400 core


subroutine vec4 filterModeType();
subroutine uniform filterModeType filterMode;

uniform sampler2D layerA; // first layer
uniform sampler2D layerB; // second layer
uniform sampler2D layerC; // third layer

uniform int quad_draw_mode;

uniform int gauss_mode;


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
uniform	float gui_gauss_w;
uniform	int gui_gauss_radius;
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

const mat3 sobel_kernel  = mat3(
                        -1.0, 0.0, +1.0,
                        -2.0, 0.0, +2.0,
                        -1.0, 0.0, +1.0
                                );

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normal_filter(){		
    vec4 c =  texture( layerA, v2QuadCoords.xy);
    if(gui_clear_alpha == 1) c = vec4(c.xyz,1);
    return c;
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

subroutine(filterModeType) vec4 mode_perspective_transform_filter(){
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
subroutine(filterModeType) vec4 mode_invert_components_filter(){
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
vec4 overlay_filter(vec4 colorA, vec4 colorB){
	return colorA * ( colorA + 2 * colorB *( 1 - colorA ) );
}

subroutine(filterModeType) vec4 mode_overlay_filter(){
    vec4 colorA = texture( layerA, v2QuadCoords.xy);
    vec4 colorB = texture( layerB, v2QuadCoords.xy);
    return overlay_filter(colorA,colorB);
}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_invert_filter(){
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

subroutine(filterModeType) vec4 mode_gauss_filter(){
    float w = gui_gauss_w;
    int radius = gui_gauss_radius;
    if(gauss_mode == 0){
            return gauss_filter(layerA,w,radius,gui_depth);
    }else if(gauss_mode == 1){
            return gauss_filter_h(layerA,w,radius,gui_depth);
    }else if(gauss_mode == 2){
            return gauss_filter_v(layerA,w,radius,gui_depth);
    }
}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
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

subroutine(filterModeType) vec4 mode_seamless_filter(){
		
    vec2 tc = v2QuadCoords.st;
    float x = tc.x;
    float y = tc.y;
    // simple seamless - GIMP like behaviour
    if(gui_seamless_mode == 1){

        vec2 r = tc  - 0.502;
        vec2 rdir    = 0.5*vec2(sign(r.x),sign(r.y));
        float rl       = 2*length(r);
        float scale  = 1.0;
        float alpha = min(scale,pow(rl,(1+20*make_seamless_radius)))/scale;

        vec4 colorA = texture( layerA, tc);
        vec4 colorB = texture( layerA, tc + rdir);
        float sq = abs(pow(2*abs(x-0.5),1+50*make_seamless_radius)) + abs(pow(2*abs(y-0.5),1+50*make_seamless_radius));

        return mix(colorA,colorB,alpha*clamp(sq,0,1));

    // mirror mode
    }else if(gui_seamless_mode == 2){
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
        highp float weight = 0;
        // loop over all atoms
        for(int i = 0 ; i < 9 ; i++){
            highp int ii = (i/3)%2;
            highp int jj = i%2;
            highp float angle = gui_seamless_random_angles[ii][jj] + gui_seamless_random_phase;

            // creating the rotation matrix
            highp mat2 rot_mat = mat2(cos(angle),sin(angle),-sin(angle),cos(angle));

            vec2 atom_pos        = vec2(atoms_x[i],atoms_y[i]);
            highp float dist     = distance(tc,atom_pos);
            highp float alpha    = 1-smoothstep(gui_seamless_random_inner_radius,gui_seamless_random_outer_radius,dist);

            weight              += alpha;
            color               += texture( layerA, rot_mat*(tc - atom_pos)+0.5)* alpha;
        }
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


subroutine(filterModeType) vec4 mode_dgaussians_filter(){

    vec4 c1 =texture(layerA,v2QuadCoords.xy);
    vec4 c2 =texture(layerB,v2QuadCoords.xy);
    vec4 dc = c1-c2;
    if(gui_mode_dgaussian ==  0){
            return dc;
    }else{
    //dc = vec4(1)*dot(dc.rgb,vec3(1))/3;
    if(gui_specular_amplifier > 0){
            return 3*gui_specular_amplifier*dc;
    }else{
            return 1+dc*gui_specular_amplifier*3;
    }	}
}

 // ----------------------------------------------------------------
//
// ----------------------------------------------------------------
vec4 contrast_filter(vec4 color,float contrast){
    float c = 1.01 * (contrast+1.0)/(1.01-contrast);
    return c * (color-0.5) + 0.5;
}
subroutine(filterModeType) vec4 mode_constrast_filter(){
    vec4 color = texture( layerA, v2QuadCoords.xy);
    vec4 icolor = contrast_filter(color,gui_specular_contrast);
    icolor = clamp(icolor,vec4(0),vec4(1));
    return clamp(icolor+gui_specular_brightness,vec4(0),vec4(1));
}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
uniform vec3 gui_gray_scale_preset;
subroutine(filterModeType) vec4 mode_gray_scale_filter(){
	
    vec4 color = texture( layerA, v2QuadCoords.xy);
    color = vec4(1)*(color.r * gui_gray_scale_preset.r + color.g * gui_gray_scale_preset.g + color.b * gui_gray_scale_preset.b);
    return clamp(color,vec4(0),vec4(1));
}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normalize_filter(){		
    vec4 color = texture( layerA, v2QuadCoords.xy);
    color.rgb =  ( color.rgb - min_color )/(max_color-min_color) ;
    color.a = 1;
    return color;
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_small_details_filter(){

    vec4 colorA   = texture( layerA, v2QuadCoords.xy);
    vec4 colorB   =-texture( layerB, v2QuadCoords.xy);
    vec4 colorC   = 1-20*gui_small_details*colorB;
    return clamp(overlay_filter(colorA,colorC*0.5),vec4(0),vec4(1));

}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_medium_details_filter(){



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
subroutine(filterModeType) vec4 mode_sharpen_blur(){
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
subroutine(filterModeType) vec4 mode_normals_step_filter(){
	
        vec4 color = 2*(texture( layerA, v2QuadCoords.xy) - 0.5);

        color.xy *= gui_normals_step;//(1+2*gui_normals_step);


        color.xyz = normalize(color.xyz);

	return color*0.5 +0.5;
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normal_to_height(){	

	float scale   = hn_min_max_scale.z;

	vec3 off  = scale*vec3(vec2(-1,1)*dxy,0);
	vec2 tex_coord = v2QuadCoords.st;


	
	float hxp  = texture(layerA,tex_coord + off.yz).x;
	float hxm = texture(layerA,tex_coord + off.xz).x;
	float hyp  = texture(layerA,tex_coord + off.zy).x;
	float hym = texture(layerA,tex_coord + off.zx).x;	
	
	float nxp   = 2*(texture(layerB,tex_coord + off.yz ).x-0.5);
	float nxm  = 2*(texture(layerB,tex_coord + off.xz ).x-0.5);
	
	float nyp   = 2*(texture(layerB,tex_coord + off.zy ).y-0.5);
	float nym  = 2*(texture(layerB,tex_coord + off.zx ).y-0.5);
	
	float h = (nxp-nxm+nyp-nym)/8.0*scale + (hxp + hxm + hyp + hym)/4.0;	


        return vec4(h);
	
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_height_to_normal(){

    const vec2 size = vec2(1.0,0.0);
    const ivec3 off = ivec3(-1,0,1);
    vec2 tex_coord =  v2QuadCoords.st;

    vec4 hc   = texture(layerA, tex_coord);
    float s11 = hc.x;    
    float s21 = textureOffset(layerA, tex_coord, off.zy).x;    
    float s12 = textureOffset(layerA, tex_coord, off.yz).x;
    vec3 va   = normalize(vec3(size.xy,gui_hn_conversion_depth*(s21-s11)));
    vec3 vb   = normalize(vec3(size.yx,gui_hn_conversion_depth*(s12-s11)));
    vec3 bump = normalize( cross(va,vb) );
    return vec4(clamp(bump*0.5 + 0.5,vec3(0),vec3(1)),1);

}



// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_sobel_filter(){
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
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normal_expansion_filter(){
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
subroutine(filterModeType) vec4 mode_smooth_filter(){
	float w     = gui_smooth_radius;
	int radius = int(w)+1;
	return gauss_filter(layerA,w,radius,1.0);
}


// ----------------------------------------------------------------
//
// ----------------------------------------------------------------

float g_intensity 	          = gui_ssao_intensity; // intensywnosc efektu
float g_scale 			  = 5*gui_ssao_depth; // dlugosc promienia prubkujacego
float g_bias 			  = 0.05*gui_ssao_bias; //  controls the width of the occlusion cone considered by the occludee.
// Pobieranie pozycji teksla w ukladzie oka
vec3 getPosition(vec2 uv){
	return vec3(uv,-g_bias*texture(layerA,uv).w);//texture(positionFBO,uv).xyz;
}
// Pobieranie normalnej
vec3 getNormal(vec2 uv){
	return normalize(texture(layerA, uv).xyz-0.5);
}

float doAmbientOcclusion(in vec2 tcoord,in vec2 uv, in vec3 p, in vec3 cnorm){
	vec3 diff = getPosition(tcoord + uv) - p;
	vec3 v = normalize(diff);
	return max(0.0,dot(cnorm,v))*g_intensity;
}
subroutine(filterModeType) vec4 mode_occlusion_filter(){		
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

subroutine(filterModeType) vec4 mode_combine_normal_height_filter(){
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

subroutine(filterModeType) vec4 mode_height_processing_filter(){
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
    return vec4(height-hmin)/(hmax-hmin);
}


out vec4 FragColor;
void main() {   
	FragColor   = filterMode();
 }
>>>>>>> 91b56da955cbfb2368f8ee03d6c7b2cdb260a88e:Sources/resources/filters.frag
