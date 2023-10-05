#version 450

struct FragNode {
  vec4 albedo;
  float depth;
  uint nextId;
};

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uOpaqueColorMap;
layout( binding = 1 ) uniform sampler2D uOpaqueDepthMap;

layout( binding = 2, r32ui )	uniform uimage2D uRootTransparency;
layout( binding = 3, std430 )	buffer uLinkedListTransparency { FragNode nodes[]; };
uniform uint uNbFragmentsMaxPerPixel;

in vec2 uv;

void main(){
    //const uint fragMax = uNbFragmentsMaxPerPixel; 
    const uint fragMax = 100;
    FragNode fragments[fragMax];

    uint indice = imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r;
    float opaqueDepth = texture(uOpaqueDepthMap,uv).r;

    uint count;
    for(count=0; count<fragMax && indice!=0 ;count++) {
        if(opaqueDepth>nodes[indice].depth) { 
            fragments[count] = nodes[indice];
            indice = fragments[count].nextId;
        }else{
            indice = nodes[indice].nextId;
            count--; 
        }
    }

    // merge sort
    FragNode leftArray[fragMax/2];
    for(uint i=1; i<=count ;i*=2){
        for(uint j=0; j<count-i ;j+=2*i){
            for(uint k=0; k<i; k++) leftArray[k] = fragments[j+k];
            
            uint id1 = 0, id2 = 0;
            uint tmp = min(j+2*i,count);
            for(uint k=j; k<tmp; k++) {
                if (j+i+id1>=tmp || (id2<i && leftArray[id2].depth>fragments[j+i+id1].depth)) {fragments[k] = leftArray[id2++];}
                else { fragments[k] = fragments[j+i+id1++]; }
            }
        }
    }

    vec4 col = vec4(texture(uOpaqueColorMap,uv).xyz,1.);
    for(uint i=0 ;i<count; i++)
        col = mix(col,fragments[i].albedo, fragments[i].albedo.a);
    fragColor = col;
}