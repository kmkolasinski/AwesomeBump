#version 400 core

subroutine vec4 filterModeType();
subroutine uniform filterModeType filterMode;

uniform sampler2D layerA; // first layer
uniform sampler2D layerB; // second layer
uniform sampler2D layerC; // third layer

uniform int gauss_mode;
uniform float make_seamless_radius;
uniform int gui_specular_radius;
uniform float gui_specular_w1;
uniform float gui_specular_w2;
uniform float gui_specular_contrast;
uniform float gui_specular_amplifier;
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
uniform float  gui_mix_normals;
uniform float  gui_smooth_radius;
uniform float gui_blend_normals;
uniform	float  gui_gauss_w;
uniform	int     gui_gauss_radius;
uniform int gui_compressed_type;
uniform int gui_mode_dgaussian;

uniform int    gui_ssao_no_iters;
uniform float gui_ssao_depth;
uniform float gui_ssao_bias;
uniform float gui_ssao_intensity;

uniform int gui_clear_alpha;

in vec2 v2QuadCoords;
vec2 dxy = vec2(1.0/max(textureSize(layerA,0).r,textureSize(layerA,0).g));

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normal_filter(){
		
                vec4 c =  texture( layerA, v2QuadCoords.xy);
                if(gui_clear_alpha == 1) c = vec4(c.xyz,1);
		return c;
}


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
subroutine(filterModeType) vec4 mode_seamless_filter(){
		
		vec2 tc = v2QuadCoords.st; 
		float x = tc.x;
		float y = tc.y;
		vec2 r = tc  - 0.501;
		vec2 rdir    = 0.5*vec2(sign(r.x),sign(r.y));
		float rl       = 2*length(r);
		float scale  = 1.0;
		float alpha = min(scale,pow(rl,(1+20*make_seamless_radius)))/scale;
			
                vec4 colorA = texture( layerA, tc);
                vec4 colorB = texture( layerA, tc + rdir);
		return mix(colorA,colorB,alpha); 
 }
 
 // ----------------------------------------------------------------
//
// ----------------------------------------------------------------
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
		dc = vec4(1)*dot(dc.rgb,vec3(1))/3;
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
		return clamp(icolor,vec4(0),vec4(1));
 }
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_gray_scale_filter(){
	
        vec4 color = texture( layerA, v2QuadCoords.xy);
	color = vec4(1)*(color.r * 0.3 + color.g * 0.59 + color.b * 0.11);
	return color;
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
	return overlay_filter(colorA,colorC*0.5);

}
// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_medium_details_filter(){

	float r1 	  = 10.0;
	float r2 	  = 0.0001;
	int radius = 10;	
        vec4 colorB   = texture( layerB, v2QuadCoords.xy);
	vec4 fcolor =(1-10*gui_medium_details*dgaussian_filter(layerB,r1,r2,radius,gui_depth))*0.5;
	
	return overlay_filter(colorB,fcolor);	
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
	//color = color/color.b;
	color.xy *= (1+2*gui_normals_step);
	color.xyz = normalize(color.xyz);
	return color*0.5 +0.5;
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normal_to_height(){	
	float vmin   = hn_min_max_scale.x;
	float vdelta = hn_min_max_scale.y;
	float scale   = hn_min_max_scale.z;

	vec3 off  = scale*vec3(vec2(-1,1)*dxy,0);
	vec2 tex_coord = v2QuadCoords.st;

	float hc    = texture(layerA,tex_coord +  off.zz).x;
	
	float hxp  = texture(layerA,tex_coord + off.yz).x;
	float hxm = texture(layerA,tex_coord + off.xz).x;
	float hyp  = texture(layerA,tex_coord + off.zy).x;
	float hym = texture(layerA,tex_coord + off.zx).x;	
	
	float nxp   = 2*(texture(layerB,tex_coord + off.yz ).x-0.5);
	float nxm  = 2*(texture(layerB,tex_coord + off.xz ).x-0.5);
	
	float nyp   = 2*(texture(layerB,tex_coord + off.zy ).y-0.5);
	float nym  = 2*(texture(layerB,tex_coord + off.zx ).y-0.5);
	
	float h = (nxp-nxm+nyp-nym)/8.0*scale + (hxp + hxm + hyp + hym)/4.0;	


	return vec4(h - vmin)/(vdelta);
	
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_height_to_normal(){

	
	const vec2 size = vec2(2.0,0.0);
	const ivec3 off = ivec3(-1,0,1);
	vec2 tex_coord =  v2QuadCoords.st;
    vec4 hc = texture(layerA, tex_coord);
    float s11 = hc.x;
    float s01 = textureOffset(layerA, tex_coord, off.xy).x;
    float s21 = textureOffset(layerA, tex_coord, off.zy).x;
    float s10 = textureOffset(layerA, tex_coord, off.yx).x;
    float s12 = textureOffset(layerA, tex_coord, off.yz).x;
    vec3 va = normalize(vec3(0.5*size.xy,10*gui_hn_conversion_depth*(s21-s11)));
    vec3 vb = normalize(vec3(0.5*size.yx,10*gui_hn_conversion_depth*(s12-s11)));
    vec3 bump = normalize( cross(va,vb) );
	

	return vec4(clamp(bump*0.5 + 0.5,vec3(0),vec3(1)),1);

}
const float w[49] = float[](
			-1, -2, -3, 0, 3, 2, 1,
            -2, -3, -4, 0, 4, 3, 2,
            -3, -4, -5, 0, 5, 4, 3,
            -4, -5, -6, 0, 6, 5, 4,
            -3, -4, -5, 0, 5, 4, 3,
            -2, -3, -4, 0, 4, 3, 2,
            -1, -2, -3, 0, 3, 2, 1 );
float w2[81]= float[]
         (
            -1, -2, -3, -4, 0, 4, 3, 2, 1,
            -2, -3, -4, -5, 0, 5, 4, 3, 2,
            -3, -4, -5, -6, 0, 6, 5, 4, 3,
            -4, -5, -6, -7, 0, 7, 6, 5, 4,
            -5, -6, -7, -8, 0, 8, 7, 6, 5,
            -4, -5, -6, -7, 0, 7, 6, 5, 4,
            -3, -4, -5, -6, 0, 6, 5, 4, 3,
            -2, -3, -4, -5, 0, 5, 4, 3, 2,
            -1, -2, -3, -4, 0, 4, 3, 2, 1     
         );
float map49(int i, int j){
	return w[j + i*7];
}

float map81(int i, int j){
	return w2[j + i*9];
}
const mat3 sobel_kernel  = mat3(
			-1.0, 0.0, +1.0,
			-2.0, 0.0, +2.0,
			-1.0, 0.0, +1.0
				);

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_sobel_filter(){
	float depth = 1.0;
	float sobel_x = 0;
	float sobel_y = 0;
	/*
	for(int i = 0 ; i < 9 ; i++){				
	for(int j = 0 ; j < 9 ; j++){								
				sobel_x   += texture(layerA,v2QuadCoords.xy+depth*vec2(i-4,j-4)*dxy).r*map81(i,j);	
				sobel_y   -=  texture(layerA,v2QuadCoords.xy+depth*vec2(i-4,j-4)*dxy).r*map81(j,i);	
		}		
	}		
	
	for(int i = 0 ; i < 7 ; i++){				
	for(int j = 0 ; j < 7 ; j++){								
				sobel_x   += texture(layerA,v2QuadCoords.xy+depth*vec2(i-3,j-3)*dxy).r*map49(i,j);	
				sobel_y   -=  texture(layerA,v2QuadCoords.xy+depth*vec2(i-3,j-3)*dxy).r*map49(j,i);	
		}		
	}	*/
	
	for(int i = 0 ; i < 3 ; i++){				
		for(int j = 0 ; j < 3 ; j++){								
				sobel_x   += texture(layerA,v2QuadCoords.xy+depth*vec2(i-1,j-1)*dxy).r*sobel_kernel[i][j];	
				sobel_y   -=  texture(layerA,v2QuadCoords.xy+depth*vec2(i-1,j-1)*dxy).r*sobel_kernel[j][i];	
		}		
	}	
	
        vec2 deriv = (vec2(sobel_y,sobel_x));
        float nz = 1;
	vec3 n = normalize(vec3(gui_basemap_amp*deriv,nz));	
	vec4 ocolor = clamp(vec4(n,0)*0.5+0.5,vec4(0),vec4(1));	
	ocolor.rgb = vec3(1-ocolor.r,ocolor.gb);
	return ocolor;
}

// ----------------------------------------------------------------
//
// ----------------------------------------------------------------
subroutine(filterModeType) vec4 mode_normal_expansion_filter(){
	vec3 sgauss = vec3(0);
	float wtotal = 0.0;
	float depth = 1.0;
	int radius   = gui_filter_radius;
        //return texture(layerA,v2QuadCoords.xy);
	if(gui_combine_normals == 0){
        vec3 n1 = normalize(2*texture(layerA,v2QuadCoords.xy).xyz-1);
	vec3 p1 = vec3(v2QuadCoords.xy,0);
	for(int i = -radius ; i <= radius ; i++){				
		for(int j = -radius ; j <= radius ; j++){												
				vec3 p2 = vec3(v2QuadCoords.xy+depth*vec2(i,j)*dxy,0);
				vec3 n2 = normalize(2*texture(layerA,p2.st).xyz-1);

                                float w = mix( length(n2.xy)+0.01 , 1/(20*gaussian(vec2(i,j),radius)*length(n2.xy)+1),gui_normal_flatting);
				wtotal    += w;
				sgauss   += n2*w;				

				
		}		
	}		
	sgauss /= (wtotal+0.00);
	
	return vec4(0.5*normalize(sgauss)+0.5,1);
	
	}else{
	
	vec3 n = normalize(texture(layerA,v2QuadCoords.xy).xyz*2-1);
	float slope = (1.0/(exp(+10*gui_mix_normals* length(n.xy))+1));
	
	vec4 a = texture(layerA,v2QuadCoords.xy);
	vec4 b = texture(layerB,v2QuadCoords.xy);
	
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

out vec4 FragColor;
void main() {   
	FragColor   = filterMode();
 }
