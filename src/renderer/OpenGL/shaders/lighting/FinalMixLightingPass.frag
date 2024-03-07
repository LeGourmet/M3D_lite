#version 450

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

layout( binding = 0 )           uniform sampler2D uLightingMap;
layout( binding = 1, r32ui )	uniform uimage2D uRootTransparency;
layout( binding = 2, std430 )	buffer uLinkedListTransparencyFrags { TranspFragNode transparencyFrags[]; };

in vec2 uv;

void main(){
    vec4 col = vec4(texture(uLightingMap,uv).xyz,1.);

    for(uint i=imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r ;i!=0; i=transparencyFrags[i].nextId)
        col = mix(col,vec4(transparencyFrags[i].emissive,transparencyFrags[i].albedo.a), transparencyFrags[i].albedo.a);

    fragColor = col;
}