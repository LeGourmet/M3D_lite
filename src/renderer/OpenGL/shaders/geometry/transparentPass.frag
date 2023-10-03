#version 450

struct FragNode {
  vec4 albedo;
  float depth;
  uint nextId;
};

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uOpaqueColorMap;
layout( binding = 1 ) uniform sampler2D uOpaqueDepthMap;

layout( binding = 2, r32ui )	uniform uimage2D uRootTransparency;                     // add counter per pixels;
layout( binding = 3, std430 )	buffer uLinkedListTransparency { FragNode nodes[]; };
layout( binding = 4, offset=0 )	uniform atomic_uint uCounterTransparency;
uniform uint uNbFragmentsMaxPerPixel;

in vec2 uv;

void main(){
    uint indice = imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r;
    FragNode frag;

    vec4 col = vec4(texture(uOpaqueColorMap,uv).xyz,1.);
    while(indice!=0){
        frag = nodes[indice];
        if(texture(uOpaqueDepthMap,uv).r>nodes[indice].depth) col = mix( col, frag.albedo, frag.albedo.a);
        indice = nodes[indice].nextId;
    }
    fragColor = col;

   /* const uint fragMax = uNbFragmentsMaxPerPixel;
    FragNode fragments[fragMax];

    uint indice = imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r;
    //uint count  = imageLoad(uCounterFragmentsTransparency, ivec2(gl_FragCoord.xy)).r;

    uint count = uNbFragmentsMaxPerPixel;
    uint i;
    for(i=0; i<count && indice!=0  ;i++) {
        fragments[i] = nodes[indice];
        indice = fragments[i].nextId;
    }
    count = i;*/

    // todo compare with opaque depth and return first id < depth
    // sorting
    /*uint step = 1;
    FragNode leftArray[fragMax/2]; //for merge sort
    
    while (step <= count){
        uint i = 0;
        while (i < count - step) {
            uint a = i;
            uint b = i + step;
            uint c = (i + step + step) >= count ? count : (i + step + step);

            for (uint k = 0; k < step; k++) leftArray[k] = fragments[a + k];
            
            uint j1 = 0;
            uint j2 = 0;
            for (uint k = a; k < c; k++) {
                if (b + j1 >= c || (j2 < step && leftArray[j2].depth > fragments[b + j1].depth)) {fragments[k] = leftArray[j2++];}
                else { fragments[k] = fragments[b + j1++]; }
            }
            i += 2 * step;
        }
        step *= 2;
    } */

    /*vec4 col = vec4(texture(uOpaqueColorMap,uv).xyz,1.);
    for(int i=0; i<count; i++) {
        //if(depth<...)return;// start from depth far form opaque and not mix it
        col = mix( col, fragments[i].albedo, fragments[i].albedo.a);
    }
    fragColor = col;*/
}