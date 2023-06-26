#version 450

#define PI				3.1415926535
#define TWO_PI			6.2831853071
#define PCF_SAMPLES     8.
#define PCF_OFFSET      0.05
#define EPS				0.0001

#define pow2(a) (a)*(a)

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPositionMap;
layout( binding = 1 ) uniform sampler2D uNormal_MetalnessMap;
layout( binding = 2 ) uniform sampler2D uAlbedo_RoughnessMap;
layout( binding = 3 ) uniform samplerCubeShadow uShadowCubeMap;

uniform vec4 uCamData;
uniform vec3 uLightPosition;
uniform vec3 uLightDirection;
uniform vec3 uLightEmissivity;
uniform vec2 uLightCosAngles;

in vec2 uv;

// ------------- fresnel -------------	:	schilk
float schlick(in float f0, in float f90, in float cosT){
    float tmp = 1.-cosT;
    return f0 + (f90 - f0) * tmp*tmp*tmp*tmp*tmp;
} 

vec3 schlick(in vec3 f0, in vec3 f90, in float cosT){ 
	float tmp = 1.-cosT;
	return f0 + (f90-f0) * tmp*tmp*tmp*tmp*tmp;
}

void main()
{
	vec4 position = texture2D(uPositionMap,uv);
	if(position.a<0.5) discard;

	vec4 albedo_roughness = texture2D(uAlbedo_RoughnessMap,uv);
	vec4 normal_metalness = texture2D(uNormal_MetalnessMap,uv);

	vec3 N = normal_metalness.xyz;
	vec3 V = normalize(uCamData.xyz-position.xyz);
	float cosNV = dot(N,V);
	if(cosNV<0.) { N *= -1.; cosNV *= -1.; }

	// ---------- LIGHT ----------
	vec3 L = uLightPosition-position.xyz;
	float lightDepth_sq = dot(L,L); 
	L = normalize(L);
	vec3 Light_Componant = uLightEmissivity * 
						   clamp((dot(-L,uLightDirection)-uLightCosAngles.y) / (uLightCosAngles.x-uLightCosAngles.y), 0., 1.) /
						   max(lightDepth_sq,EPS); 

	float cosNL = dot(N,L);
	if(cosNL<0.) discard;

	// --- SHADOW ---
	vec3 T = cross(N,vec3(1.,0.,0.));
	if(length(T)<0.1) T = cross(N,vec3(0.,1.,0.));
	T = normalize(T);
	vec3 B = normalize(cross(N,T));

	// --- PCF => todo implement pcss
	float shadow  = 0.;
	for(float x = -PCF_OFFSET; x<PCF_OFFSET; x += PCF_OFFSET/(PCF_SAMPLES*0.5))
		for(float y = -PCF_OFFSET; y<PCF_OFFSET; y += PCF_OFFSET/(PCF_SAMPLES*0.5)){
				vec3 fp = position.xyz + T*x + B*y + L*clamp(0.05*(cosNL),0.05,0.1); // N better but bug black zones 
				vec3 fpL = fp-uLightPosition;
				float lightDepth = length(fpL);
				float shadowDepth = texture(uShadowCubeMap,vec4(normalize(fpL),1.))*uCamData.a;
				shadow += (lightDepth > shadowDepth) ? 0. : 1.;
			}
	shadow /= (PCF_SAMPLES * PCF_SAMPLES);

	// ---------- SHADING ----------
	vec3 H = normalize(V+L);
	float cosHV = dot(H,V);
	float cosHL = dot(H,L);
	float cosHN = dot(H,N);

	// --- dielectic ---
	float f90 = albedo_roughness.a * (2.*cosHL*cosHL+0.5);
    vec3 dielectric = albedo_roughness.xyz * schlick(1.,f90,cosNL) * schlick(1.,f90,cosNV) * (1.-albedo_roughness.a*0.51/1.51) / PI;

	// --- conductor ---   
	float r = albedo_roughness.a * albedo_roughness.a;
    float r2 = r*r;
    
    // use fresnel normal incidence for m.ior and 1.
    vec3 F0 = mix(vec3(0.04),albedo_roughness.xyz,normal_metalness.a);
    vec3 F = schlick(F0,vec3(1.),cosHL);
    float D = r2/(PI*pow2(cosHN*cosHN*(r2-1.)+1.));
    float V2 = 0.5/(cosNL*sqrt((cosNV-cosNV*r2)*cosNV+r2) + cosNV*sqrt((cosNL-cosNL*r2)*cosNL+r2));
    vec3 conductor = F*D*V2;

	fragColor = vec4(((1.-F)*dielectric+conductor) * Light_Componant * shadow * cosNL,1.);
}