#version 450

// ------ references ------ 
// - [1] : https://www.iryoku.com/smaa/
// - [2] : https://github.com/UTwelve/3AReal/blob/master/ReshadeForFFXIV/Reshade/Shaders/SMAA.fxh

layout( location = 0 ) out vec3 fragColor;

layout( binding = 0 ) uniform sampler2D uSrcTexture;

uniform vec2 uInvSrcRes;

in vec2 uv;

#if defined(SMAA_PRESET_LOW)
#define SMAA_THRESHOLD 0.15
#define SMAA_MAX_SEARCH_STEPS 4
#define SMAA_DISABLE_DIAG_DETECTION
#define SMAA_DISABLE_CORNER_DETECTION
#elif defined(SMAA_PRESET_MEDIUM)
#define SMAA_THRESHOLD 0.1
#define SMAA_MAX_SEARCH_STEPS 8
#define SMAA_DISABLE_DIAG_DETECTION
#define SMAA_DISABLE_CORNER_DETECTION
#elif defined(SMAA_PRESET_HIGH)
#define SMAA_THRESHOLD 0.1
#define SMAA_MAX_SEARCH_STEPS 16
#define SMAA_MAX_SEARCH_STEPS_DIAG 8
#define SMAA_CORNER_ROUNDING 25
#elif defined(SMAA_PRESET_ULTRA)
#define SMAA_THRESHOLD 0.05
#define SMAA_MAX_SEARCH_STEPS 32
#define SMAA_MAX_SEARCH_STEPS_DIAG 16
#define SMAA_CORNER_ROUNDING 25
#endif

//#define SMAA_DEPTH_THRESHOLD (0.1 * SMAA_THRESHOLD)
//#define SMAA_MAX_SEARCH_STEPS 16				=> [0,112]
//#define SMAA_MAX_SEARCH_STEPS_DIAG 8			=> [0,20]
//#define SMAA_CORNER_ROUNDING 25				=> [0,100]
//#define SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR 2.0
//#define SMAA_PREDICATION 0
//#define SMAA_PREDICATION_THRESHOLD 0.01
//#define SMAA_PREDICATION_SCALE 2.0			=> [1,5]
//#define SMAA_PREDICATION_STRENGTH 0.4			=> [0,1]
//#define SMAA_REPROJECTION 0
//#define SMAA_REPROJECTION_WEIGHT_SCALE 30.0	=> [0,80]

/*
#define SMAA_AREATEX_MAX_DISTANCE 16
#define SMAA_AREATEX_MAX_DISTANCE_DIAG 20
#define SMAA_AREATEX_PIXEL_SIZE (1.0 / float2(160.0, 560.0))
#define SMAA_AREATEX_SUBTEX_SIZE (1.0 / 7.0)
#define SMAA_SEARCHTEX_SIZE float2(66.0, 33.0)
#define SMAA_SEARCHTEX_PACKED_SIZE float2(64.0, 16.0)
#define SMAA_CORNER_ROUNDING_NORM (float(SMAA_CORNER_ROUNDING) / 100.0)
*/

/*
#define SMAATexture2D(tex) sampler2D tex
#define SMAATexturePass2D(tex) tex
#define SMAASampleLevelZero(tex, coord) textureLod(tex, coord, 0.0)
#define SMAASampleLevelZeroPoint(tex, coord) textureLod(tex, coord, 0.0)
#define SMAASampleLevelZeroOffset(tex, coord, offset) textureLodOffset(tex, coord, 0.0, offset)
#define SMAASample(tex, coord) texture(tex, coord)
#define SMAASamplePoint(tex, coord) texture(tex, coord)
#define SMAASampleOffset(tex, coord, offset) texture(tex, coord, offset)
#define SMAA_FLATTEN
#define SMAA_BRANCH
#define lerp(a, b, t) mix(a, b, t)
#define saturate(a) clamp(a, 0.0, 1.0)
#define mad(a, b, c) fma(a, b, c)
#define SMAAGather(tex, coord) textureGather(tex, coord)
*/

void main()
{
	vec3 pixelColor = texture(uSrcTexture,uv).xyz;

	// input => smaa*edgeDetection => egesTex => smaaBlendingWeightCalculation => BlendTex => SMAANeighborhoodBlending => output

	// DO SOME SMAA STUFF

	fragColor = pixelColor;
}