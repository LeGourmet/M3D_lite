#version 450

#define PI				3.1415926535897932384626433832795
#define TWO_PI			6.2831853071795864769252867665590
#define INV_SQRT_TWO	0.70710678118654752440084436210485
#define EPS				0.0001

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPosition_MetalnessMap;
layout( binding = 1 ) uniform sampler2D uNormal_RoughnessMap;
layout( binding = 2 ) uniform sampler2D uAlbedoMap;

uniform vec3 uCamPos;
uniform vec4 uLightPositionType;	 // position  + type
uniform vec4 uLightDirectionInner;	 // direction + cosInnerAngle
uniform vec4 uLightEmissivityOuter;  // emission  + cosOuterAngle

in vec2 uv;

// ------------- fresnel -------------	:	schilk
float getFresnel(float f0, float cosHV){ 
	float tmp = 1.-cosHV;
	return f0 + (1 - f0) * tmp*tmp*tmp*tmp*tmp;
}

vec3 getFresnel(vec3 f0, float cosHV){ return vec3(getFresnel(f0.x,cosHV),getFresnel(f0.y,cosHV),getFresnel(f0.z,cosHV)); }

// ---------- specular BRDF ----------	:	GGX+smith
float getSpecular(float a2, float cosNV, float cosNL, float cosNH){
	a2 *= a2; // re-maping

	// *** G : smith ***
    float tmpG = 1.-a2;
    float G = 2.*cosNV*cosNL/ (cosNL * sqrt(a2+tmpG*cosNV*cosNV) + cosNV * sqrt(a2+tmpG*cosNL*cosNL)); // care 0

	// *** D : GGX ***
	float tmpD = cosNH*cosNH*(a2-1.)+1.;
	float D = a2/(PI*tmpD*tmpD);		// care == 0

	return G*D/(4.*cosNV*cosNL);
}

// ---------- diffuse BRDF ----------	:	oren Nayar
float getDiffuse(float a, float cosNV, float cosNL, vec3 N, vec3 V, vec3 L ){
	float r_OrenNayar = atan(a)*INV_SQRT_TWO;
	float r2_OrenNayar = r_OrenNayar*r_OrenNayar;
	float To = acos(cosNV);
	float Ti = acos(cosNL);
	float PHI = dot(normalize(V-N*cosNL),normalize(L-N*cosNV));
	float A = 1.-0.5*(r2_OrenNayar/(r2_OrenNayar+0.33));
	float B	= 0.45*(r2_OrenNayar/(r2_OrenNayar+0.09));
	return (A+( B * max(0.,PHI) * sin(max(Ti,To)) * tan(min(Ti,To)) ) ) / PI;
}

void getDirectional(inout vec3 p_lightDir, inout vec3 p_lightComponant){
	p_lightDir = normalize(-uLightDirectionInner.xyz);
	p_lightComponant = uLightEmissivityOuter.xyz;
}

void getPonctualLight(in vec3 fragPos, inout vec3 p_lightDir, inout vec3 p_lightComponant){
	p_lightDir = uLightPositionType.xyz-fragPos;
	float attenuation = 1./max(dot(p_lightDir,p_lightDir),EPS); // or 1./(a*d*d + b*d + c)
	p_lightDir = normalize(p_lightDir);
	float intensity = clamp((dot(-p_lightDir, normalize(uLightDirectionInner.xyz))-uLightEmissivityOuter.w) / (uLightDirectionInner.w-uLightEmissivityOuter.w), 0., 1.);
	p_lightComponant = uLightEmissivityOuter.xyz * intensity * attenuation; 
}

void main()
{
	vec4 albedo = texture2D(uAlbedoMap,uv);
	if(albedo.a<0.5) discard;

	vec4 position_metalness = texture2D(uPosition_MetalnessMap,uv);
	vec4 normal_roughness = texture2D(uNormal_RoughnessMap,uv);

	vec3 N = normal_roughness.xyz;
	vec3 V = normalize(uCamPos-position_metalness.xyz);
	float cosNV = dot(N,V);
	if(cosNV<0.) { N *= -1.; cosNV=dot(N,V); }

	vec3 L,Light_Componant;
	if(uLightPositionType.w < 0.5){ getDirectional(L,Light_Componant); }
	else{ getPonctualLight(position_metalness.xyz, L, Light_Componant); }

	float cosNL = dot(N,L);
	if(cosNL<0.) discard;
	
	vec3 H = normalize(V+L);
	float cosNH = dot(N,H);
	float cosHV = dot(H,V);

	float specular = getSpecular(normal_roughness.a*normal_roughness.a,cosNV,cosNL,cosNH);
	float diffuse = getDiffuse(normal_roughness.a,cosNV,cosNL,N,V,L);

	vec3 dielectricComponent = albedo.xyz * mix(diffuse,specular,getFresnel(0.04,cosHV)); //we can use ior with ((1-ior)/(1+ior))^2 that emplace 0.04
	vec3 MetalComponent = getFresnel(albedo.xyz,cosHV) * specular;

	fragColor = vec4(mix(dielectricComponent,MetalComponent,position_metalness.a) * Light_Componant * cosNL, 1.); 
}