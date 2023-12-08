#version 450

struct FragNode {
    vec4 albedo;
    vec3 position;
    vec3 normal;
    vec3 emissive;
    float roughness;
    float depth;
    uint nextId;
};

layout( binding = 0 )           uniform sampler2D uOpaqueDepthMap;
layout( binding = 1, r32ui)		uniform uimage2D uRootTransparency;
layout( binding = 2, std430)	buffer uLinkedListTransparency { FragNode nodes[]; };

in vec2 uv;

bool insert( inout uint headId, in uint currentId ){
    uint iPreviousId = 0;

    for(uint i=headId; i!=currentId ;i=nodes[i].nextId) {    
        if(nodes[currentId].depth>=nodes[i].depth){     
        //if(nodes[currentId].depth<=nodes[i].depth){     
            if(iPreviousId==0){
                imageStore(uRootTransparency, ivec2(gl_FragCoord.xy), uvec4(currentId));
                headId = currentId;
            }else{ nodes[iPreviousId].nextId = currentId; }
            nodes[currentId].nextId = i;
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
        uint futurId = nodes[currentId].nextId;

        if(opaqueDepth>nodes[currentId].depth) { // sort (insertion)
            if(insert(headId,currentId)) { nodes[previousId].nextId=futurId; }
            else { previousId = currentId; }
        } else { 
            if(previousId==0) { // filter
                imageStore(uRootTransparency, ivec2(gl_FragCoord.xy), uvec4(nodes[currentId].nextId));
                headId = nodes[currentId].nextId;
            } else { nodes[previousId].nextId=nodes[currentId].nextId; }
        }

        currentId=futurId;
    }
}