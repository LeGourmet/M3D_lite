#version 450

#define PI				3.1415926535
#define PCF_SAMPLES     8
#define PCF_OFFSET      0.1

#define pow2(a) (a)*(a)
#define pow5(a) (a)*(a)*(a)*(a)*(a)

struct TranspFragNode {
    vec4 albedo;
    vec3 normal;
    float metalness;
    float roughness;
    vec3 emissive;
    float depth;
    uint nextId;
};

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 )           uniform sampler2D uOpaqueAlbedoMap;
layout( binding = 1 )           uniform sampler2D uOpaqueNormalMap;
layout( binding = 2 )           uniform sampler2D uOpaqueMetalnessRoughnessMap;
layout( binding = 3 )           uniform sampler2D uOpaqueDepthMap;
layout( binding = 4 )           uniform sampler2DShadow uShadowMap;
layout( binding = 5 )           uniform samplerCubeShadow uShadowCubeMap;
//layout( binding = 4 )           uniform sampler2D uOpaqueShadowMap;
layout( binding = 6, r32ui )	uniform uimage2D  uRootTransparency;
layout( binding = 7, std430 )	buffer            uLinkedListTransparencyFrags { TranspFragNode transparencyFrags[]; };
//layout( binding = 8, std430 )	buffer            uLinkedListTransparencyShadow { vec3 transparencyShadows[]; };

uniform vec3 uCamPos;
uniform mat4 uInvMatrix_VP;
uniform float uShadowFar; 
uniform mat4 uLightMatrix_VP;

uniform vec3 uLightPosition;
uniform vec3 uLightDirection;
uniform vec3 uLightEmissivity;
uniform vec2 uLightCosAngles;
uniform bool uLightTypePoint;

in vec2 uv;

float schlick(in float f0, in float f90, in float cosT){return f0 + (f90-f0) * pow5(1.-cosT); } 
vec3 schlick(in vec3 f0, in vec3 f90, in float cosT){ return f0 + (f90-f0) * pow5(1.-cosT); }

vec3 computeShadow(in vec3 N, in vec3 L, in vec3 fragPos){
    float shadow = 0.;
	float cosNL = max(0.,dot(N,L));

    if(uLightTypePoint){
        #if PCF_SAMPLES < 2
		    vec3 fp = fragPos + L*clamp(0.05*cosNL,0.05,0.1); // offset => use N or L ; factors should depend of something 
		    vec3 fpL = fp-uLightPosition;
		    float lightDepth = length(fpL);
		    shadow = (lightDepth > texture(uShadowCubeMap,vec4(normalize(fpL),1.))*uShadowFar) ? 0. : 1.;
	    #else
		    vec3 T = cross(N,vec3(1.,0.,0.));
		    if(length(T)<0.1) T = cross(N,vec3(0.,1.,0.));
		    T = normalize(T);
		    vec3 B = normalize(cross(N,T));

		    float pcf_step = 2.*PCF_OFFSET/float(PCF_SAMPLES-1);
		    for(int i=0; i<PCF_SAMPLES ;i++)
			    for(int j=0; j<PCF_SAMPLES ;j++){
				    vec3 fp = fragPos + T*(float(i)*pcf_step-PCF_OFFSET) + B*(float(j)*pcf_step-PCF_OFFSET);
					     fp += L*clamp(0.05*cosNL,0.05,0.1); // offset => use N or L ; factors should depend of something 
				    vec3 fpL = fp-uLightPosition;
				    float lightDepth = length(fpL);
				    float shadowDepth = texture(uShadowCubeMap,vec4(normalize(fpL),1.))*uShadowFar;
				    shadow += (lightDepth > shadowDepth) ? 0. : 1.; // pcss => facteur compris entre 0 et 1 compute
			    }
		    shadow /= float(pow2(PCF_SAMPLES));
	    #endif
    }else{
        #if PCF_SAMPLES < 2
		    vec3 fp = fragPos + L*clamp(0.05*cosNL,0.05,0.1); // offset => use N or L ; factors should depend of something  // N*EPS + L*max(0.5,cosNL)*1000./uCamData.a;
		    vec3 fpLS = (uLightMatrix_VP * vec4(fp,1.)).xyz;
		    shadow = (fpLS.z < 1.) ? texture(uShadowMap,fpLS) : 1.; // pcss => facteur compris entre 0 et 1 compute
	    #else
		    vec3 T = cross(N,vec3(1.,0.,0.));
		    if(length(T)<0.1) T = cross(N,vec3(0.,1.,0.));
		    T = normalize(T);
		    vec3 B = normalize(cross(N,T));

		    float pcf_step = 2.*PCF_OFFSET/float(PCF_SAMPLES-1);
		    for(int i=0; i<PCF_SAMPLES ;i++)
			    for(int j=0; j<PCF_SAMPLES ;j++){
				    vec3 fp = fragPos + T*(float(i)*pcf_step-PCF_OFFSET) + B*(float(j)*pcf_step-PCF_OFFSET);
					     fp += L*clamp(0.05*cosNL,0.05,0.1); // offset => use N or L ; factors should depend of something  // N*EPS + L*max(0.5,cosNL)*1000./uCamData.a;
				    vec3 fpLS = (uLightMatrix_VP * vec4(fp,1.)).xyz;
				    shadow += (fpLS.z < 1.) ? texture(uShadowMap,fpLS) : 1.; // pcss => facteur compris entre 0 et 1 compute
			    }
		    shadow /= float(pow2(PCF_SAMPLES));
	    #endif
    }
    return vec3(shadow);
}

vec3 brdf(in vec3 N, in vec3 H, in vec3 L, in vec3 V, in vec3 albedo, in float metalness, in float roughness){
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
        vec4 fragPos = uInvMatrix_VP*vec4(2.*uv-1.,2.*transparencyFrags[i].depth-1.,1.);
        if (abs(fragPos.a)>0.001)  fragPos /= fragPos.a;

        vec3 lightingIntensity = uLightEmissivity;// * transparencyShadows[i];
        vec3 L = -uLightDirection;
        if(uLightTypePoint) computeDataForPointLight(L,lightingIntensity,fragPos.xyz);

        vec3 V = normalize(uCamPos-fragPos.xyz);
	    vec3 H = normalize(V+L);
        vec3 N = transparencyFrags[i].normal;
        if(dot(N,V)<0.) N = -N;

        transparencyFrags[i].emissive += brdf(N,H,L,V,transparencyFrags[i].albedo.xyz,transparencyFrags[i].metalness,transparencyFrags[i].roughness) * lightingIntensity;
    }

    // --------- OPAQUE ---------
    vec3 N = texture(uOpaqueNormalMap,uv).xyz; // convert ?
	if(N.x==0. && N.y==0. && N.z==0.) discard;

	vec3 albedo = texture(uOpaqueAlbedoMap,uv).xyz;
	vec2 metalnessRoughness = texture(uOpaqueMetalnessRoughnessMap,uv).xy;

    vec4 fragPos = uInvMatrix_VP*vec4(2.*uv-1.,2.*texture(uOpaqueDepthMap,uv).x-1.,1.);
    if (abs(fragPos.a)>0.001)  fragPos /= fragPos.a;

    vec3 lightingIntensity = uLightEmissivity; //* texture(uOpaqueShadowMap,uv).xyz;
    vec3 L = -uLightDirection;
    if(uLightTypePoint) computeDataForPointLight(L,lightingIntensity,fragPos.xyz);

    vec3 V = normalize(uCamPos-fragPos.xyz);
    vec3 H = normalize(V+L);
    if(dot(N,V)<0.) N = -N;

	lightingIntensity *= computeShadow(N,L,fragPos.xyz);

    fragColor = vec4(brdf(N,H,L,V,albedo,metalnessRoughness.x,metalnessRoughness.y) * lightingIntensity, 1.);
}