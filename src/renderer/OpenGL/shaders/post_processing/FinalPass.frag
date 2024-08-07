#version 460

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uTexture;
layout( binding = 1 ) uniform sampler2D uBloom;
layout( binding = 2 ) uniform sampler2D uAgXTextureLUT;

uniform float uBloomPower;

in vec2 uv;

// https://github.com/MrLixm/AgXc/blob/main/reshade/reshade-shaders/Shaders/AgX.fx
// https://github.com/MrLixm/AgXc/blob/main/obs/obs-script/AgX.hlsl

#define INPUT_HIGHLIGHT_GAIN_GAMMA  1.
#define INPUT_HIGHLIGHT_GAIN        1.
#define INPUT_SATURATION            1.
#define INPUT_GAMMA                 1.
#define INPUT_EXPOSURE              0.
#define PUNCH_GAMMA                 2.2
#define PUNCH_SATURATION            1.
#define PUNCH_EXPOSURE              0.

#define LUT_BLOCK_SIZE 32
#define LUT_DIMENSIONS ivec2(LUT_BLOCK_SIZE*LUT_BLOCK_SIZE, LUT_BLOCK_SIZE)
#define LUT_PIXEL_SIZE 1./LUT_DIMENSIONS

const mat3 agx_compressed_matrix = mat3(
    0.842479062253094, 0.0423282422610123, 0.0423756549057051,
    0.0784335999999992,  0.878468636469772,  0.0784336,
    0.0792237451477643, 0.0791661274605434, 0.879142973793104
);

const mat3 agx_compressed_matrix_inverse = mat3(
    1.19687900512017, -0.0528968517574562, -0.0529716355144438,
    -0.0980208811401368, 1.15190312990417, -0.0980434501171241,
    -0.0990297440797205, -0.0989611768448433, 1.15107367264116
);


vec3 powsafe(vec3 color, float power) { return pow(abs(color), vec3(power)) * sign(color); }
float powsafe(float color, float power) { return pow(abs(color), power) * sign(color); }
float getLuminance(vec3 color){ return dot(color, vec3(0.2126390058715103, 0.7151686787677559, 0.07219231536073371)); }
vec3 saturation(vec3 color, float saturationAmount) { return mix( vec3(getLuminance(color)), color, saturationAmount); }

vec3 convertOpenDomainToNormalizedLog2(vec3 color, float minimum_ev, float maximum_ev){
    color = max(vec3(0.), color);
    color = vec3( (color.x<0.00003051757) ? (0.00001525878+color.x) : (color.x), (color.y<0.00003051757) ? (0.00001525878+color.y) : (color.y), (color.z<0.00003051757) ? (0.00001525878+color.z) : (color.z));
    color = clamp( log2(color/0.18), vec3(minimum_ev), vec3(maximum_ev) );
    return (color-minimum_ev)/(maximum_ev-minimum_ev);
}

void main(){
    vec3 col = texture(uTexture,uv).xyz;//+uBloomPower*texture(uBloom,uv).xyz;

    // --- Open Grading ---
    col += col * vec3(powsafe(getLuminance(col), INPUT_HIGHLIGHT_GAIN_GAMMA)) * INPUT_HIGHLIGHT_GAIN;
    col = saturation(col, INPUT_SATURATION);
    col = powsafe(col, INPUT_GAMMA);
    col *= powsafe(2., INPUT_EXPOSURE);
    
	// --- DRT ---
    // * agx log * 
	col = max(vec3(0.),col);
    col = agx_compressed_matrix*col;
    col = convertOpenDomainToNormalizedLog2(col, -12.47393, 4.026069);
    col = clamp(col, 0., 1.);

    // * agX lut *
    vec3 lut3D = col*(LUT_BLOCK_SIZE-1);
    vec2 lut2D_0 = (vec2(floor(lut3D.z)*LUT_BLOCK_SIZE+lut3D.x,lut3D.y)+0.5)*LUT_PIXEL_SIZE;
    vec2 lut2D_1 = (vec2(ceil(lut3D.z)*LUT_BLOCK_SIZE+lut3D.x,lut3D.y)+0.5)*LUT_PIXEL_SIZE;
    col = mix( texture(uAgXTextureLUT, lut2D_0).xyz, texture(uAgXTextureLUT, lut2D_1).xyz, fract(lut3D.z) );
    col = powsafe(col, 2.2);
    col = agx_compressed_matrix_inverse*col;

    // --- ODT ---
    col = powsafe(col, 1./2.2);

    // --- Display grading ---
    col = powsafe(col, PUNCH_GAMMA);
    col = saturation(col, PUNCH_SATURATION);
    col *= powsafe(2., PUNCH_EXPOSURE);

	fragColor = vec4(col,1.);
}