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

	// ---------- diffuse BRDF ----------	:	oren Nayar
	normal_roughness.a = 0.2;
	position_metalness.a  = 0.8;

	float r_OrenNayar = atan(normal_roughness.a)*INV_SQRT_TWO;
	float r2_OrenNayar = r_OrenNayar*r_OrenNayar;
	float To = acos(cosNV);
	float Ti = acos(cosNL);
	float PHI = dot(normalize(V-N*cosNL),normalize(L-N*cosNV));
	float A = 1.-0.5*(r2_OrenNayar/(r2_OrenNayar+0.33));
	float B	= 0.45*(r2_OrenNayar/(r2_OrenNayar+0.09));
	vec3 diffuseBRDF = albedo.xyz * (A+( B * max(0.,PHI) * sin(max(Ti,To)) * tan(min(Ti,To)) ) ) / PI;

	// ---------- specular BRDF ----------	:	GGX+schilk+smith
	float r2_GGX = normal_roughness.a*normal_roughness.a;
	r2_GGX *= r2_GGX;
	
	// *** F : Schlick ***
	/*float tmpF = 1.- cosHV;
	vec3 F0 = mix(vec3(0.04),albedo.xyz,position_metalness.a);
	vec3 F = F0 + ( 1. - F0 ) * tmpF*tmpF*tmpF*tmpF*tmpF;
	
	// *** G : smith ***
    float tmpG = 1.-r2_GGX;
    float G = 2.*cosNV*cosNL/ (cosNL * sqrt(r2_GGX+tmpG*cosNV*cosNV) + cosNV * sqrt(r2_GGX+tmpG*cosNL*cosNL)); // care 0
	
	//SmithGGXCorrelated => caca sur les bord 
	//float G = 0.5 / (cosNL * sqrt((-cosNV * r2_GGX + cosNV) * cosNV + r2_GGX) + cosNV * sqrt((-cosNL * r2_GGX + cosNL) * cosNL + r2_GGX));

	// *** D : GGX ***
	float tmpD = cosNH*cosNH*(r2_GGX-1.)+1.;
	float D = r2_GGX/(PI*tmpD*tmpD);		// care == 0

	vec3 specularBRDF =  F*G*D/(4.*cosNV*cosNL);*/

	// simplification
	float tmpF = 1.- cosHV;
	float tmpG = 1.- r2_GGX;
	float tmpD = cosNH*cosNH*(r2_GGX-1.)+1.;
	vec3 F0 = mix(vec3(0.04),albedo.xyz,position_metalness.a);
	vec3 F = F0 + ( 1. - F0 ) * tmpF*tmpF*tmpF*tmpF*tmpF;
	vec3 specularBRDF =  F*r2_GGX/(TWO_PI*(cosNL * sqrt(r2_GGX+tmpG*cosNV*cosNV) + cosNV * sqrt(r2_GGX+tmpG*cosNL*cosNL))*tmpD*tmpD+EPS);

	fragColor = vec4(((1.-position_metalness.a)*diffuseBRDF + specularBRDF) * Light_Componant * cosNL, 1.); 
}