#version 450

#define PI				3.1415926535

#define pow2(a) (a)*(a)
#define pow5(a) (a)*(a)*(a)*(a)*(a)

struct FragNode {
    vec4 albedo;
    vec3 position;
    vec3 normal;
    vec3 emissive;
    float roughness;
    float depth;
    uint nextId;
};

layout( binding = 0, r32ui )	uniform uimage2D uRootTransparency;
layout( binding = 1, std430 )	buffer uLinkedListTransparency { FragNode nodes[]; };
layout( binding = 2 )           uniform sampler2DShadow uShadowMap;

uniform vec4 uCamData;
uniform mat4 uLightMatrix_VP;
uniform vec3 uLightDirection;
uniform vec3 uLightEmissivity;

in vec2 uv;

float schlick(in float f0, in float f90, in float cosT){return f0 + (f90-f0) * pow5(1.-cosT); } 
vec3 schlick(in vec3 f0, in vec3 f90, in float cosT){ return f0 + (f90-f0) * pow5(1.-cosT); }

void main(){
    for(uint i=imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r ;i!=0; i=nodes[i].nextId){
        vec3 V = normalize(uCamData.xyz-nodes[i].position);
        
        // ---------- LIGHT ----------
        vec3 L = -uLightDirection;
        float shadow = 1.;
        
        // ---------- SHADING ----------
	    vec3 H = normalize(V+L);
        vec3 N = nodes[i].normal * ((dot(nodes[i].normal,V)<0.) ? -1. : 1.);
	    //if(dot(N,V)<0. && dot(N,L)<0.) N = -N;

	    float cosNV = max(1e-5,abs(dot(N,V)));
	    float cosNL = max(0.,dot(N,L));
	    float cosHL = max(0.,dot(H,L));
	    float cosHN = max(0.,dot(H,N));

	    float r = nodes[i].roughness * nodes[i].roughness;
        float r2 = r*r;
	
	    // --- dielectic ---
	    float Rr = r*2.f*cosHL*cosHL+0.5f;
        float Fl = pow5(1.f-cosNL);
        float Fv = pow5(1.f-cosNV);
	    vec3 dielectric = nodes[i].albedo.xyz * ((1.f-0.5f*Fl) * (1.f-0.5f*Fv) + Rr*(Fl+Fv+Fl*Fv*(Rr-1.f)))/PI;

	    // --- conductor ---
	    vec3 F = schlick(mix(vec3(schlick(0.04,1.,max(0.,cosHL))), nodes[i].albedo.xyz, 0.), vec3(1.), cosHL); // todo use metalness
        float D = r2/max(1e-5,(PI*pow2(cosHN*cosHN*(r2-1.)+1.)));
        float V2 = 0.5/max(1e-5,(cosNL*sqrt((cosNV-cosNV*r2)*cosNV+r2) + cosNV*sqrt((cosNL-cosNL*r2)*cosNL+r2)));
        vec3 conductor = F*D*V2;

        nodes[i].emissive += mix(dielectric,conductor,0.) * uLightEmissivity * shadow * cosNL; // todo use metalness
    }
}