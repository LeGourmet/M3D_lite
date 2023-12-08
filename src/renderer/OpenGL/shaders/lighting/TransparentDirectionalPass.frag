#version 450

#define PI				3.1415926535

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

float schlick(in float f0, in float f90, in float cosT){
    float tmp = 1.-cosT;
    return f0 + (f90 - f0) * tmp*tmp*tmp*tmp*tmp;
} 

void main(){
    for(uint i=imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r ;i!=0; i=nodes[i].nextId){
        vec3 V = normalize(uCamData.xyz-nodes[i].position);
        vec3 L = -uLightDirection;
        
        float shadow = 1.;

        vec3 H = normalize(V+L);
        vec3 N = nodes[i].normal * ((dot(nodes[i].normal,V)<0.) ? -1. : 1.);

        float cosNL = dot(N,L);
	    float cosNV = dot(N,V);
	    float cosHV = dot(H,V);
	    float cosHL = dot(H,L);
	    float cosHN = dot(H,N);

	    float f90 = nodes[i].roughness * (2.*cosHL*cosHL+0.5);
        vec3 dielectric = nodes[i].albedo.xyz * schlick(1.,f90,cosNL) * schlick(1.,f90,cosNV) * (1.-nodes[i].roughness*0.51/1.51) / PI;
	
        nodes[i].emissive += dielectric * uLightEmissivity * shadow * max(0.,cosNL);
    }
}