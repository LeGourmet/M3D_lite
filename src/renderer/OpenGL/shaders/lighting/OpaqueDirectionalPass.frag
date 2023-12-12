#version 450

#define PI				3.1415926535
#define TWO_PI			6.2831853071
#define PCF_SAMPLES     8.
#define PCF_OFFSET      0.1
#define EPS				0.0001

#define pow2(a) (a)*(a)
#define pow5(a) (a)*(a)*(a)*(a)*(a)

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


float schlick(in float f0, in float f90, in float cosT){return f0 + (f90-f0) * pow5(1.-cosT); } 
vec3 schlick(in vec3 f0, in vec3 f90, in float cosT){ return f0 + (f90-f0) * pow5(1.-cosT); }

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

	// ---------- SHADING ----------
	vec3 H = normalize(V+L);
	if(dot(N,V)<0. && dot(N,L)<0.) N = -N;

	float cosNV = max(1e-5,abs(dot(N,V)));
	float cosNL = max(0.,dot(N,L));
	float cosHL = max(0.,dot(H,L));
	float cosHN = max(0.,dot(H,N));

	float r = albedo_roughness.a * albedo_roughness.a;
    float r2 = r*r;
	
	// --- dielectic ---
	float Rr = r*2.f*cosHL*cosHL+0.5f;
    float Fl = pow5(1.f-cosNL);
    float Fv = pow5(1.f-cosNV);
	vec3 dielectric = albedo_roughness.xyz * ((1.f-0.5f*Fl) * (1.f-0.5f*Fv) + Rr*(Fl+Fv+Fl*Fv*(Rr-1.f)))/PI;

	// --- conductor ---
	vec3 F = schlick(mix(vec3(schlick(0.04,1.,max(0.,cosHL))), albedo_roughness.xyz, normal_metalness.a), vec3(1.), cosHL);
    float D = r2/max(1e-5,(PI*pow2(cosHN*cosHN*(r2-1.)+1.)));
    float V2 = 0.5/max(1e-5,(cosNL*sqrt((cosNV-cosNV*r2)*cosNV+r2) + cosNV*sqrt((cosNL-cosNL*r2)*cosNL+r2)));
    vec3 conductor = F*D*V2;

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

	fragColor = vec4(mix(dielectric,conductor,normal_metalness.a) * uLightEmissivity * shadow * cosNL,1.);
}