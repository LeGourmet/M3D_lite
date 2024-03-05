#version 450

#define PI				3.1415926535

#define pow2(a) (a)*(a)
#define pow5(a) (a)*(a)*(a)*(a)*(a)

struct TranspFragNode {
    vec4 albedo;
    vec3 position;
    vec3 normal;
    vec3 emissive;
    float metalness;
    float roughness;
    float depth;
    uint nextId;
};

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 )           uniform sampler2D uOpaquePositionMap;
layout( binding = 1 )           uniform sampler2D uOpaqueNormalMetalnessMap;
layout( binding = 2 )           uniform sampler2D uOpaqueAlbedoRoughnessMap;
layout( binding = 3 )           uniform sampler2D uOpaqueShadowMap;
layout( binding = 4, r32ui )	uniform uimage2D  uRootTransparency;
layout( binding = 5, std430 )	buffer            uLinkedListTransparencyFrags { TranspFragNode transparencyFrags[]; };
layout( binding = 6, std430 )	buffer            uLinkedListTransparencyShadow { vec3 transparencyShadows[]; };

uniform vec3 uCamPos;
uniform vec3 uLightPosition;
uniform vec3 uLightDirection;
uniform vec3 uLightEmissivity;
uniform vec2 uLightCosAngles;
uniform bool uLightTypePoint;

in vec2 uv;

float schlick(in float f0, in float f90, in float cosT){return f0 + (f90-f0) * pow5(1.-cosT); } 
vec3 schlick(in vec3 f0, in vec3 f90, in float cosT){ return f0 + (f90-f0) * pow5(1.-cosT); }

vec3 brdf(in vec3 N, in vec3 H, in vec3 L, in vec3 V, in vec3 albedo, in float roughness, in float metalness){
    float cosNV = max(1e-5,abs(dot(N,V)));
	float cosNL = max(0.,dot(N,L));
	float cosHL = max(0.,dot(H,L));
	float cosHN = max(0.,dot(H,N));

    float r = clamp(roughness, 0.01, 0.99);
		  r = pow2(r);
    float r2 = pow2(r);

    // --- diffuse ---
	float Rr = r*2.*cosHL*cosHL+0.5;
    float Fl = pow5(1.-cosNL);
    float Fv = pow5(1.-cosNV);
	vec3 diffuse = albedo * ((1.-0.5*Fl) * (1.-0.5*Fv) + Rr*(Fl+Fv+Fl*Fv*(Rr-1.)))/PI;

	// --- specular ---
    vec3 f0 = mix(vec3(schlick(0.04,1.,cosHL)), albedo, metalness);
    vec3 F = schlick(f0, vec3(1.), cosHL);
	float D = r2/max(1e-5,(PI*pow2(cosHN*cosHN*(r2-1.)+1.)));
    float V2 = 0.5/max(1e-5,(cosNL*sqrt((cosNV-cosNV*r2)*cosNV+r2) + cosNV*sqrt((cosNL-cosNL*r2)*cosNL+r2)));
    vec3 specular = F*D*V2;

    vec3 dielectric = mix(diffuse,specular,schlick(0.66,1.,cosHL)); // 0.66 not physicaly base but better results 
	vec3 conductor = specular;

    return mix(dielectric,conductor,metalness) * cosNL;
}

// --------- TRANSPARENCY BSDF add ? ---------
/*  float cosNL = max(0.,dot(-N, transmitRay.getDirection()));
	float DielF = schlick(0., 1., max(0.,dot(H,V)));
                    
    float XL = sqrt(r2 + (1. - r2) * cosNL * cosNL);
    float XV = sqrt(r2 + (1. - r2) * cosNV * cosNV);
    float G1L = 2. * cosNL / max(1e-5, (cosNL + XL));
    float G1V = 2. * cosNV / max(1e-5, (cosNV + XV));
    float G2 = 2. * cosNL * cosNV / max(1e-5, (cosNV * XL + cosNL * XV));
	
    vec3 transmit = sqrt(vec3(albedo)) * (1.-DielF)*G2*pow2(p_ni/p_no)/max(1e-5,G1L);  // D should be somewere + need to reintroduce pdf */

void computeDataForPointLight(inout vec3 L, inout vec3 lightingIntensity, in vec3 fragPos) {
    L = uLightPosition-fragPos;
	float lightDepth_sq = dot(L,L); 
	L = normalize(L);
	lightingIntensity *= clamp((dot(-L,uLightDirection)-uLightCosAngles.y) / (uLightCosAngles.x-uLightCosAngles.y), 0., 1.) / max(lightDepth_sq,1e-5);
}

void main(){
    // --------- TRANSPARENT ---------
    for(uint i=imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r ;i!=0; i=transparencyFrags[i].nextId){
        vec3 lightingIntensity = uLightEmissivity * transparencyShadows[i];
        vec3 L = -uLightDirection;
        if(uLightTypePoint) computeDataForPointLight(L,lightingIntensity,transparencyFrags[i].position);

        vec3 V = normalize(uCamPos-transparencyFrags[i].position);
	    vec3 H = normalize(V+L);
        vec3 N = transparencyFrags[i].normal;
        if(dot(N,V)<0.) N = -N;

        transparencyFrags[i].emissive += brdf(N,H,L,V,transparencyFrags[i].albedo.xyz,transparencyFrags[i].roughness,transparencyFrags[i].metalness) * lightingIntensity;
    }

    // --------- OPAQUE ---------
    vec4 opaquePosition = texture(uOpaquePositionMap,uv);
	if(opaquePosition.a<0.5) discard;

	vec4 opaqueNormalMetalness = texture(uOpaqueNormalMetalnessMap,uv);
	vec4 opaqueAlbedoRoughness = texture(uOpaqueAlbedoRoughnessMap,uv);

    vec3 lightingIntensity = uLightEmissivity * texture(uOpaqueShadowMap,uv).xyz;
    vec3 L = -uLightDirection;
    if(uLightTypePoint) computeDataForPointLight(L,lightingIntensity,opaquePosition.xyz);

	vec3 V = normalize(uCamPos-opaquePosition.xyz);
    vec3 H = normalize(V+L);
	vec3 N = opaqueNormalMetalness.xyz;
    if(dot(N,V)<0.) N = -N;

    fragColor = vec4(brdf(N,H,L,V,opaqueAlbedoRoughness.xyz,opaqueAlbedoRoughness.a,opaqueNormalMetalness.a) * lightingIntensity, 1.);
}