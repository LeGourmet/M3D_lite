#version 450

struct TranspFragNode {
    vec4 albedo;
    vec3 normal;
	float metalness;
    float roughness;
    vec3 emissive;
	vec3 directLighting;
    float depth;
    uint nextId;
};

layout( binding = 0 )           uniform sampler2D uOpaqueDepthMap;
layout( binding = 1, r32ui)		uniform uimage2D uRootTransparency;
layout( binding = 2, std430)	buffer uLinkedListTransparencyFrags { TranspFragNode transparencyFrags[]; };

in vec2 uv;

bool insert( inout uint headId, in uint currentId ){
    uint iPreviousId = 0;

    for(uint i=headId; i!=currentId ;i=transparencyFrags[i].nextId) {    
        if(transparencyFrags[currentId].depth<=transparencyFrags[i].depth){ 
            if(iPreviousId==0){
                imageStore(uRootTransparency, ivec2(gl_FragCoord.xy), uvec4(currentId));
                headId = currentId;
            }else{ transparencyFrags[iPreviousId].nextId = currentId; }
            transparencyFrags[currentId].nextId = i;
            return true;
        }
        iPreviousId = i;
    }
    return false;
}

void main(){
    float opaqueDepth = texture(uOpaqueDepthMap, uv).r;
    uint headId = imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r;
    
    uint currentId = headId;
    uint previousId = 0; 

    while(currentId!=0){
        uint futurId = transparencyFrags[currentId].nextId;

        if(opaqueDepth>transparencyFrags[currentId].depth) { // sort (insertion)
            if(insert(headId,currentId)) { transparencyFrags[previousId].nextId=futurId; }
            else { previousId = currentId; }
        } else { 
            if(previousId==0) { // filter
                imageStore(uRootTransparency, ivec2(gl_FragCoord.xy), uvec4(transparencyFrags[currentId].nextId));
                headId = transparencyFrags[currentId].nextId;
            } else { transparencyFrags[previousId].nextId=transparencyFrags[currentId].nextId; }
        }

        currentId=futurId;
    }
}