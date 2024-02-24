#version 450

#define PI				3.1415926535
#define PCF_SAMPLES     8
#define PCF_OFFSET      0.05
#define EPS				0.0001

#define pow2(a) (a)*(a)
#define pow5(a) (a)*(a)*(a)*(a)*(a)

struct FragNode {
    vec4 albedo;
    vec3 position;
    vec3 normal;
    vec3 emissive;
    float metalness;
    float roughness;
    float depth;
    uint nextId;
};

layout( binding = 0, r32ui )	uniform uimage2D uRootTransparency;
layout( binding = 1, std430 )	buffer uLinkedListTransparency { FragNode nodes[]; };
layout( binding = 2 )           uniform samplerCubeShadow uShadowCubeMap;

uniform vec4 uCamData;
uniform vec3 uLightPosition;
uniform vec3 uLightDirection;
uniform vec3 uLightEmissivity;
uniform vec2 uLightCosAngles;

in vec2 uv;

float schlick(in float f0, in float f90, in float cosT){return f0 + (f90-f0) * pow5(1.-cosT); } 
vec3 schlick(in vec3 f0, in vec3 f90, in float cosT){ return f0 + (f90-f0) * pow5(1.-cosT); }

void main(){
    for(uint i=imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r ;i!=0; i=nodes[i].nextId){
        vec3 V = normalize(uCamData.xyz-nodes[i].position);
        
        // ---------- LIGHT ----------
        vec3 L = uLightPosition-nodes[i].position;
	    float lightDepth_sq = dot(L,L); 
	    L = normalize(L);
	    vec3 Light_Componant = uLightEmissivity * 
						       clamp((dot(-L,uLightDirection)-uLightCosAngles.y) / (uLightCosAngles.x-uLightCosAngles.y), 0., 1.) /
						       max(lightDepth_sq,EPS);

        // ---------- SHADING ----------
	    vec3 H = normalize(V+L);
        vec3 N = nodes[i].normal * ((dot(nodes[i].normal,V)<0.) ? -1. : 1.); // if double side not discard than flip before compute
	    //if(dot(N,V)<0. && dot(N,L)<0.) N = -N;

	    float cosNV = max(1e-5,abs(dot(N,V)));
	    float cosNL = max(0.,dot(N,L));
	    float cosHL = max(0.,dot(H,L));
	    float cosHN = max(0.,dot(H,N));

        float r = clamp(nodes[i].roughness, 0.01, 0.99);
		      r = pow(r,1.75); //pow2(r*r);
        float r2 = pow2(r);
	
	    // --- dielectic ---
	    float Rr = r*2.*cosHL*cosHL+0.5;
        float Fl = pow5(1.-cosNL);
        float Fv = pow5(1.-cosNV);
	    vec3 dielectric = nodes[i].albedo.xyz * ((1.-0.5*Fl) * (1.-0.5*Fv) + Rr*(Fl+Fv+Fl*Fv*(Rr-1.)))/PI;

	    // --- conductor ---
        vec3 f0 = mix(vec3(schlick(0.04,1.,cosHL)), nodes[i].albedo.xyz, nodes[i].metalness);
        vec3 F = schlick(f0, vec3(1.), cosHL);
	    float D = r2/max(1e-5,(PI*pow2(cosHN*cosHN*(r2-1.)+1.)));
        float V2 = 0.5/max(1e-5,(cosNL*sqrt((cosNV-cosNV*r2)*cosNV+r2) + cosNV*sqrt((cosNL-cosNL*r2)*cosNL+r2)));
        vec3 conductor = F*D*V2;

		// ---------- SHADOW ----------
		// use slope scale bias => dot(L,N)
		// use adaptive Bias Based on Depth => base on difference znear/zfar for light source
		// use bias that scale on resalution of the shadow map
		// use pcf and/or pcss

		float shadow = 0.;

		#if PCF_SAMPLES < 2
			vec3 fp = nodes[i].position + L*clamp(0.05*cosNL,0.05,0.1); // offset => use N or L ; factors should depend of something 
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
					vec3 fp = nodes[i].position + T*(float(i)*pcf_step-PCF_OFFSET) + B*(float(j)*pcf_step-PCF_OFFSET);
						 fp += L*clamp(0.05*cosNL,0.05,0.1); // offset => use N or L ; factors should depend of something 
					vec3 fpL = fp-uLightPosition;
					float lightDepth = length(fpL);
					float shadowDepth = texture(uShadowCubeMap,vec4(normalize(fpL),1.))*uCamData.a;
					shadow += (lightDepth > shadowDepth) ? 0. : 1.; // pcss => facteur compris entre 0 et 1 compute
				}
			shadow /= float(pow2(PCF_SAMPLES));
		#endif

		// ---------- FINAL MIX ----------
        nodes[i].emissive += mix(dielectric,conductor,nodes[i].metalness) * uLightEmissivity * shadow * cosNL;
    }
}