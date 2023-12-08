#version 450

#define PI				3.1415926535
#define TWO_PI			6.2831853071
#define PCF_SAMPLES     8.
#define PCF_OFFSET      0.1
#define EPS				0.0001

#define pow2(a) (a)*(a)

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPositionMap;
layout( binding = 1 ) uniform sampler2D uNormal_MetalnessMap;
layout( binding = 2 ) uniform sampler2D uAlbedo_RoughnessMap;
layout( binding = 3 ) uniform sampler2DShadow uShadowMap;

uniform vec4 uCamData;
uniform mat4 uLightMatrix_VP;
uniform vec3 uLightDirection;
uniform vec3 uLightEmissivity;

in vec2 uv;

// use slope scale bias => dot(L,N)
// use adaptive Bias Based on Depth => base on difference znear/zfar for light source
// use bias that scale on resalution of the shadow map


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
	vec4 position = texture(uPositionMap,uv);
	if(position.a<0.5) discard;

	vec4 albedo_roughness = texture(uAlbedo_RoughnessMap,uv);
	vec4 normal_metalness = texture(uNormal_MetalnessMap,uv);

	vec3 N = normal_metalness.xyz;
	vec3 V = normalize(uCamData.xyz-position.xyz);

	// ---------- LIGHT ----------
	vec3 L = -uLightDirection;

	float cosNL = dot(N,L);
	float cosNV = dot(N,V);
	if(cosNL<0. && cosNV<0.) N=-N;
	cosNL = abs(cosNL);
	cosNV = abs(cosNV);
	//if(cosNV<0.) { N=-N; cosNV=-cosNV; cosNL=-cosNL; }
	//if(cosNL<0.) { return; }

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
				//vec3 fp = position.xyz + T*x + B*y + N*EPS + L*max(0.5,cosNL)*1000./uCamData.a;
				vec3 fpLS = (uLightMatrix_VP * vec4(fp,1.)).xyz;
				shadow += (fpLS.z < 1.) ? texture(uShadowMap,fpLS) : 1.;
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

    vec3 F = schlick(albedo_roughness.xyz,vec3(1.),cosHL);
    float D = r2/max(1e-5,(PI*pow2(cosHN*cosHN*(r2-1.)+1.)));
    float V2 = 0.5/max(1e-5,(cosNL*sqrt((cosNV-cosNV*r2)*cosNV+r2) + cosNV*sqrt((cosNL-cosNL*r2)*cosNL+r2)));
    vec3 conductor = F*D*V2;

	fragColor = vec4(mix(dielectric,conductor,normal_metalness.a) * uLightEmissivity * shadow * cosNL,1.);
}