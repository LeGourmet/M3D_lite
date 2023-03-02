#version 450
#define PI				3.1415926535897932384626433832795
#define TWO_PI			6.2831853071795864769252867665590
#define INV_SQRT_TWO	0.70710678118654752440084436210485
#define EPS				0.00001

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPosition_MetalnessMap;
layout( binding = 1 ) uniform sampler2D uNormal_RoughnessMap;
layout( binding = 2 ) uniform sampler2D uAlbedoMap;

uniform vec3 uCamPos;
uniform vec4 uLightPosition;	 // position  + type
uniform vec4 uLightDirection;	 // direction + angle Min (entre -1 et 1)
uniform vec4 uLightEmissivity;   // emission  + angle Max (entre -1 et 1)
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

void main()
{
	vec4 albedo = texture2D(uAlbedoMap,uv);
	if(albedo.a<0.5) discard;

	vec4 position_metalness = texture2D(uPosition_MetalnessMap,uv);
	vec4 normal_roughness = texture2D(uNormal_RoughnessMap,uv);

	vec3 L,Light_Componant;
	if(uLightPosition.w < 0.5){
		L = normalize(-uLightDirection.xyz);
		Light_Componant = uLightEmissivity.xyz;
	}else{
		L = uLightPosition.xyz-position_metalness.xyz;
		float attenuation = 1./dot(L,L); // or 1./(a*d*d + b*d + c)
		L = normalize(L);
		float intensity = clamp((dot(-L, normalize(uLightDirection.xyz)) - uLightDirection.w) / (uLightEmissivity.w-uLightDirection.w), 0., 1.);
		Light_Componant = uLightEmissivity.xyz * intensity * attenuation; 
	}

	vec3 N = normal_roughness.xyz;
	float cosNL = dot(N,L); // cti
	if(cosNL <= 0.) discard;

	vec3 V = normalize(uCamPos-position_metalness.xyz);
	float cosNV = dot(N,V); // cto
	if(cosNV <= 0.) discard;
	
	vec3 H = normalize(V+L);
	float cosNH = dot(N,H);
	float cosHV = dot(H,V);

	normal_roughness.a = 0.2;
	position_metalness.a  = 0.8;

	float specular = getSpecular(normal_roughness.a*normal_roughness.a,cosNV,cosNL,cosNH);
	float diffuse = getDiffuse(normal_roughness.a,cosNV,cosNL,N,V,L);

	vec3 dielectricComponent = albedo.xyz * mix(diffuse,specular,getFresnel(0.04,cosHV)); //we can use ior with ((1-ior)/(1+ior))^2 that emplace 0.04
	vec3 MetalComponent = getFresnel(albedo.xyz,cosHV) * specular;

	fragColor = vec4(mix(dielectricComponent,MetalComponent,position_metalness.a) * Light_Componant * cosNL, 1.); 
}