#version 450

#define PI				3.1415926535

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
//layout( binding = 2 )         buffer shadowPerChannelPerFragment

uniform vec4 uCamData;
uniform vec3 uLightPosition;
uniform vec3 uLightDirection;
uniform vec3 uLightEmissivity;
uniform vec2 uLightCosAngles;
uniform bool uLightTypePoint;

in vec2 uv;

float schlick(in float f0, in float f90, in float cosT){return f0 + (f90-f0) * pow5(1.-cosT); } 
vec3 schlick(in vec3 f0, in vec3 f90, in float cosT){ return f0 + (f90-f0) * pow5(1.-cosT); }

void main(){
    for(uint i=imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r ;i!=0; i=nodes[i].nextId){
        vec3 V = normalize(uCamData.xyz-nodes[i].position);
        
        // ---------- LIGHT ----------
        vec3 L = -uLightDirection;
        vec3 light_componant = uLightEmissivity;

        if (uLightTypePoint) {
            L = uLightPosition-nodes[i].position;
	        float lightDepth_sq = dot(L,L); 
	        L = normalize(L);
	        light_componant *= clamp((dot(-L,uLightDirection)-uLightCosAngles.y) / (uLightCosAngles.x-uLightCosAngles.y), 0., 1.) / max(lightDepth_sq,0.0001);
        }
        
        // ---------- SHADING ----------
	    vec3 H = normalize(V+L);
        vec3 N = nodes[i].normal;
        if(dot(N,V)<0.) N = -N;

	    float cosNV = max(1e-5,abs(dot(N,V)));
	    float cosNL = max(0.,dot(N,L));
	    float cosHL = max(0.,dot(H,L));
	    float cosHN = max(0.,dot(H,N));

        float r = clamp(nodes[i].roughness, 0.01, 0.99);
		      r = pow2(r);
        float r2 = pow2(r);
	
	    // --- diffuse ---
	    float Rr = r*2.*cosHL*cosHL+0.5;
        float Fl = pow5(1.-cosNL);
        float Fv = pow5(1.-cosNV);
	    vec3 diffuse = nodes[i].albedo.xyz * ((1.-0.5*Fl) * (1.-0.5*Fv) + Rr*(Fl+Fv+Fl*Fv*(Rr-1.)))/PI;

	    // --- specular ---
        vec3 f0 = mix(vec3(schlick(0.04,1.,cosHL)), nodes[i].albedo.xyz, nodes[i].metalness);
        vec3 F = schlick(f0, vec3(1.), cosHL);
	    float D = r2/max(1e-5,(PI*pow2(cosHN*cosHN*(r2-1.)+1.)));
        float V2 = 0.5/max(1e-5,(cosNL*sqrt((cosNV-cosNV*r2)*cosNV+r2) + cosNV*sqrt((cosNL-cosNL*r2)*cosNL+r2)));
        vec3 specular = F*D*V2;

		// --- transmit ---
		/*
		float cosNL = max(0.,dot(-N, transmitRay.getDirection()));
		float DielF = schlick(0., 1., max(0.,dot(H,V)));
                    
        float XL = sqrt(r2 + (1. - r2) * cosNL * cosNL);
        float XV = sqrt(r2 + (1. - r2) * cosNV * cosNV);
        float G1L = 2. * cosNL / max(1e-5, (cosNL + XL));
        float G1V = 2. * cosNV / max(1e-5, (cosNV + XV));
        float G2 = 2. * cosNL * cosNV / max(1e-5, (cosNV * XL + cosNL * XV));
		*/
        //vec3 transmit = sqrt(vec3(albedo)) * (1.-DielF)*G2/**pow2(p_ni/p_no)*//max(1e-5,G1L); 
		//vec3 transmit = sqrt(vec3(albedo)) * (1.-DielF) * D *


		// ---------- SHADOW ----------
		vec3 shadow = vec3(1.);

		// ---------- FINAL MIX ----------
	    vec3 dielectric = mix(diffuse,specular,schlick(0.66,1.,cosHL));
	    vec3 conductor = specular;

        nodes[i].emissive += mix(dielectric,conductor,nodes[i].metalness) * light_componant * shadow * cosNL;
    }
}