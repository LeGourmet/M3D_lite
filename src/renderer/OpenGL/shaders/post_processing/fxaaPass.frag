#version 450

// adapted from: https://github.com/kosua20/Rendu/blob/master/resources/common/shaders/screens/fxaa.frag

layout( location = 0 ) out vec3 fragColor;

layout( binding = 0 ) uniform sampler2D uSrcTexture;

uniform vec2 uInvSrcRes;

in vec2 uv;

#define EDGE_THRESHOLD_MIN 0.0624
#define EDGE_THRESHOLD_MAX 0.125
#define SUBPIXEL_QUALITY   0.75
#define NB_SEARCH_STEPS	   12
const float AA_QUALITY[ 12 ]	= {1.,1.,1.,1.,1.,1.5,2.,2.,2.,2.,4.,8.};

const vec3 luma = vec3(0.299,0.587,0.114);
float rgb2luma(const vec3 rgb){ return dot(rgb,luma); }

void main()
{
	vec3 pixelColor = texture(uSrcTexture,uv).xyz;

	// --- edge detection ---
	float lumaC = rgb2luma( pixelColor );
	float lumaD = rgb2luma( texture(uSrcTexture, uv+vec2( 0.,-1.)*uInvSrcRes).xyz );
	float lumaU = rgb2luma( texture(uSrcTexture, uv+vec2( 0., 1.)*uInvSrcRes).xyz );
	float lumaL = rgb2luma( texture(uSrcTexture, uv+vec2(-1., 0.)*uInvSrcRes).xyz );
	float lumaR = rgb2luma( texture(uSrcTexture, uv+vec2( 1., 0.)*uInvSrcRes).xyz );

	float lumaMax = max(lumaC,max(lumaD,max(lumaU,max(lumaL,lumaR))));
	float lumaMin = min(lumaC,min(lumaD,min(lumaU,max(lumaL,lumaR))));
	float lumaRange = lumaMax-lumaMin;

	if(lumaRange<max(EDGE_THRESHOLD_MIN,lumaMax*EDGE_THRESHOLD_MAX)){ fragColor = pixelColor; return; }

	// --- vertical/horizontal edge test ---
	float lumaDL = rgb2luma( texture(uSrcTexture, uv+vec2(-1.,-1.)*uInvSrcRes).xyz );
	float lumaUR = rgb2luma( texture(uSrcTexture, uv+vec2( 1., 1.)*uInvSrcRes).xyz );
	float lumaUL = rgb2luma( texture(uSrcTexture, uv+vec2(-1., 1.)*uInvSrcRes).xyz );
	float lumaDR = rgb2luma( texture(uSrcTexture, uv+vec2( 1.,-1.)*uInvSrcRes).xyz );

	float lumaDU   = lumaD  + lumaU;
	float lumaLR   = lumaL  + lumaR;

	float lumaLeftCorner  = lumaDL + lumaUL;
	float lumaDownCorner  = lumaDL + lumaDR;
	float lumaRightCorner = lumaDR + lumaUR;
	float lumaUpCorner	  = lumaUR + lumaUL;

	float edgeHorizontal = abs(lumaLeftCorner-2.*lumaL) + abs(lumaDU-2.*lumaC)*2. + abs(lumaRightCorner-2.*lumaR);
	float edgeVertical	 = abs(lumaUpCorner-2.*lumaU)   + abs(lumaLR-2.*lumaC)*2. + abs(lumaDownCorner-2.*lumaD);

	// --- choose edge orientation ---
	bool isHorizontal    = (edgeHorizontal >= edgeVertical);
	float luma1			 = (isHorizontal) ? lumaD : lumaL;
	float luma2			 = (isHorizontal) ? lumaU : lumaR;
	float stepLength 	 = (isHorizontal) ? uInvSrcRes.y : uInvSrcRes.x;
	vec2 offset			 = (isHorizontal) ? vec2(uInvSrcRes.x,0.) : vec2(0.,uInvSrcRes.y); // VTX inverse x and y mais bug
	vec2 currentUV		 = ((isHorizontal) ? vec2(0.,stepLength*0.5) : vec2(stepLength*0.5,0.))+uv;

	float gradient1		 = abs(luma1-lumaC);
	float gradient2		 = abs(luma2-lumaC);
	float gradientScaled = max(gradient1,gradient2)*0.25;

	float lumaLocalAvg;
	if(gradient1>=gradient2){ lumaLocalAvg=(luma1+lumaC)*0.5; stepLength = -stepLength; }
	else					{ lumaLocalAvg=(luma2+lumaC)*0.5; }
	
	// --- end-of-edge search ---
	vec2 uv1 = currentUV - offset*AA_QUALITY[0];
	vec2 uv2 = currentUV + offset*AA_QUALITY[0];
	
	float lumaVar1 = rgb2luma( texture(uSrcTexture, uv1).xyz ) - lumaLocalAvg;
	float lumaVar2 = rgb2luma( texture(uSrcTexture, uv2).xyz ) - lumaLocalAvg;
	bool isDone1=false, isDone2=false, isDoneBoth=false;
	
	for(int i=1; i<NB_SEARCH_STEPS && !isDoneBoth ;i++){
		if(!isDone1) lumaVar1 = rgb2luma(texture(uSrcTexture,uv1.xy).xyz) - lumaLocalAvg;
		if(!isDone2) lumaVar2 = rgb2luma(texture(uSrcTexture,uv2.xy).xyz) - lumaLocalAvg;
			
		isDone1 = abs(lumaVar1) >= gradientScaled;
		isDone2 = abs(lumaVar2) >= gradientScaled;
		if(!isDone1) uv1 -= offset*AA_QUALITY[i];
		if(!isDone2) uv2 += offset*AA_QUALITY[i];
		isDoneBoth = isDone1 && isDone2;
	}

	float dist1 = (isHorizontal) ? uv.x-uv1.x : uv.y-uv1.y;
	float dist2 = (isHorizontal) ? uv2.x-uv.x : uv2.y-uv.y;
	
	// --- sub-pixel aliasing test ---
	float lumaAvg = (2.*(lumaDU+lumaLR)+lumaLeftCorner+lumaRightCorner)/12.;
	float subPixelOffset1 = clamp(abs(lumaAvg-lumaC)/lumaRange,0.,1.);
	float subPixelOffset2 = (3.-2.*subPixelOffset1)*subPixelOffset1*subPixelOffset1;
	float subPixelOffset  = subPixelOffset2*subPixelOffset2*SUBPIXEL_QUALITY;

	bool correctVariation = (dist1<dist2) ? ((lumaVar1<0.) != (lumaC<lumaLocalAvg)) : ((lumaVar2<0.) != (lumaC<lumaLocalAvg));
	float pixelOffset = (correctVariation) ? 0.5-min(dist1,dist2)/(dist1+dist2) : 0.;

	vec2 finalUV = uv + max(pixelOffset,subPixelOffset) * ((isHorizontal) ? vec2(0.,stepLength) : vec2(stepLength,0.));
	fragColor = texture(uSrcTexture,finalUV).xyz;
}