#version 450

#define PI				3.1415926535

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

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 )           uniform sampler2D uIndirectLightingMap;
layout( binding = 1, r32ui )	uniform uimage2D uRootTransparency;
layout( binding = 2, std430 )	buffer uLinkedListTransparency { FragNode nodes[]; };

in vec2 uv;

void main(){
    vec4 col = vec4(texture(uIndirectLightingMap,uv).xyz,1.);
    
    /*vec3 finalColor = vec3(0.);
    vec3 rayColor = vec3(1.);
    for(uint i=imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r ;i!=0; i=nodes[i].nextId){
        finalColor += nodes[i].albedo.a*nodes[i].emissive*rayColor;
        rayColor *= (1.-nodes[i].albedo.a)*nodes[i].albedo.xyz;
    }
    
    fragColor = vec4(finalColor + texture(uIndirectLightingMap,uv).xyz*rayColor, 1.);*/

    for(uint i=imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r ;i!=0; i=nodes[i].nextId){
        col = mix(col,vec4(nodes[i].emissive,nodes[i].albedo.a), nodes[i].albedo.a);
    }
    fragColor = col;
}