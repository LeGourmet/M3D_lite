#version 450

#define PI				3.1415926535
#define PCF_SAMPLES     8
#define PCF_OFFSET      0.05
#define EPS				0.0001

#define pow2(a) (a)*(a)
#define pow5(a) (a)*(a)*(a)*(a)*(a)

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
	vec3 L = uLightPosition-position.xyz;
	float lightDepth_sq = dot(L,L); 
	L = normalize(L);
	vec3 Light_Componant = uLightEmissivity * 
						   clamp((dot(-L,uLightDirection)-uLightCosAngles.y) / (uLightCosAngles.x-uLightCosAngles.y), 0., 1.) /
						   max(lightDepth_sq,EPS);

	// ---------- SHADING ----------
	vec3 H = normalize(V+L);
	//if(dot(N,V)<0. && dot(N,L)<0.) N = -N; // if double side not discard than flip before compute
	if(dot(N,V)<0.) N = -N;
	//fragColor = vec4(vec3(normal_metalness.a),1.);
	//fragColor = vec4(vec3(1.-albedo_roughness.a),1.);
	//return;

	float cosNV = max(1e-5,abs(dot(N,V)));
	float cosNL = max(0.,dot(N,L));
	float cosHL = max(0.,dot(H,L));
	float cosHN = max(0.,dot(H,N));

	float r = clamp(albedo_roughness.a, 0.01, 0.99);
		  r = pow2(r);
    float r2 = pow2(r);
	
	// --- diffuse ---
	float Rr = r*2.*cosHL*cosHL+0.5;
    float Fl = pow5(1.-cosNL);
    float Fv = pow5(1.-cosNV);
	vec3 diffuse = albedo_roughness.xyz * ((1.-0.5*Fl) * (1.-0.5*Fv) + Rr*(Fl+Fv+Fl*Fv*(Rr-1.)))/PI;

	// --- specular ---
	vec3 f0 = mix(vec3(schlick(0.04,1.,cosHL)), albedo_roughness.xyz, normal_metalness.a);
    vec3 F = schlick(f0, vec3(1.), cosHL);
	float D = r2/max(1e-5,(PI*pow2(cosHN*cosHN*(r2-1.)+1.)));
    float V2 = 0.5/max(1e-5,(cosNL*sqrt((cosNV-cosNV*r2)*cosNV+r2) + cosNV*sqrt((cosNL-cosNL*r2)*cosNL+r2)));
    vec3 specular = F*D*V2;

	// ---------- SHADOW ----------
	// use slope scale bias => dot(L,N)
	// use adaptive Bias Based on Depth => base on difference znear/zfar for light source
	// use bias that scale on resalution of the shadow map
	// use pcf and/or pcss

	float shadow = 0.;

	#if PCF_SAMPLES < 2
		vec3 fp = position.xyz + L*clamp(0.05*cosNL,0.05,0.1); // offset => use N or L ; factors should depend of something 
		vec3 fpL = fp-uLightPosition;
		float lightDepth = length(fpL);
		shadow = (lightDepth > texture(uShadowCubeMap,vec4(normalize(fpL),1.))*uCamData.a) ? 0. : 1.;
	#else
		vec3 T = cross(N,vec3(1.,0.,0.));
		if(length(T)<0.1) T = cross(N,vec3(0.,1.,0.));
		T = normalize(T);
		vec3 B = normalize(cross(N,T));

		float pcf_step = 2.*PCF_OFFSET/float(PCF_SAMPLES-1);
		for(int i=0; i<PCF_SAMPLES ;i++)
			for(int j=0; j<PCF_SAMPLES ;j++){
				vec3 fp = position.xyz + T*(float(i)*pcf_step-PCF_OFFSET) + B*(float(j)*pcf_step-PCF_OFFSET);
					 fp += L*clamp(0.05*cosNL,0.05,0.1); // offset => use N or L ; factors should depend of something 
				vec3 fpL = fp-uLightPosition;
				float lightDepth = length(fpL);
				float shadowDepth = texture(uShadowCubeMap,vec4(normalize(fpL),1.))*uCamData.a;
				shadow += (lightDepth > shadowDepth) ? 0. : 1.; // pcss => facteur compris entre 0 et 1 compute
			}
		shadow /= float(pow2(PCF_SAMPLES));
	#endif

	// ---------- FINAL MIX ----------
	vec3 dielectric = mix(diffuse,specular,schlick(0.65,1.,cosHL));
	vec3 conductor = specular;

	fragColor = vec4(mix(dielectric,conductor,normal_metalness.a) * Light_Componant * shadow * cosNL,1.);
}