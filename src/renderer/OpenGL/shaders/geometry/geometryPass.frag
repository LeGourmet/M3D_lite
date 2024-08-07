#version 460

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

layout( location = 0 ) out vec3 opaqueAlbedo;
layout( location = 1 ) out vec3 opaqueNormal;
layout( location = 2 ) out vec2 opaqueMetalnessRoughness;
layout( location = 3 ) out vec3 opaqueEmissive;

layout( binding = 1 )			uniform sampler2D uAlbedoMap;
layout( binding = 2 )			uniform sampler2D uNormalMap;
layout( binding = 3 )			uniform sampler2D uMetalnessRoughnessMap;
layout( binding = 4 )			uniform sampler2D uEmissiveMap;
layout( binding = 5, r32ui)		uniform uimage2D uRootTransparency;
layout( binding = 6, std430)	buffer uLinkedListTransparencyFrags { TranspFragNode transparencyFrags[]; };
layout( binding = 7, offset=0)	uniform atomic_uint uCounterTransparency;

uniform vec4 uAlbedo;
uniform float uMetalness;
uniform float uRoughness;
uniform vec3 uEmissiveColor;
uniform float uEmissiveStrength;
uniform float uAlphaCutOff;

uniform bool uHasAlbedoMap;
uniform bool uHasNormalMap;
uniform bool uHasMetalnessRoughnessMap;
uniform bool uHasEmissiveMap;

uniform uint uNbTranspFragsMax;

in vec2 uv;
in vec3 fragNormal;
in mat3 TBN;

void main(){
	vec4 albedo = (uHasAlbedoMap ? pow(texture(uAlbedoMap,uv),vec4(2.2,2.2,2.2,1.)) : vec4(1.))*uAlbedo;

	vec3 normal = normalize(uHasNormalMap ? (TBN*(texture(uNormalMap,uv).xyz*2.-1.)) : ((gl_FrontFacing) ? fragNormal : -fragNormal));
	vec2 MetalnessRoughness = (uHasMetalnessRoughnessMap ? texture(uMetalnessRoughnessMap,uv).zy : vec2(1.)) * vec2(uMetalness,uRoughness);
	vec3 emissivity = (uHasEmissiveMap ? pow(texture(uEmissiveMap,uv).xyz,vec3(2.2)) : vec3(1.)) * uEmissiveColor * uEmissiveStrength;

	if(uAlphaCutOff<1.){ // opaque
		if(albedo.a<uAlphaCutOff) discard;

		opaqueAlbedo = albedo.xyz;
		opaqueNormal = normal;
		opaqueMetalnessRoughness = MetalnessRoughness;
		opaqueEmissive = emissivity;

	}else{	// transparent
		uint currentId = atomicCounterIncrement(uCounterTransparency);

		if( currentId < uNbTranspFragsMax ) {
			uint nextId = imageAtomicExchange(uRootTransparency, ivec2(gl_FragCoord.xy), currentId);
			transparencyFrags[currentId].albedo = albedo;
			transparencyFrags[currentId].normal = normal;
			transparencyFrags[currentId].metalness = MetalnessRoughness.x;
			transparencyFrags[currentId].roughness = MetalnessRoughness.y;
			transparencyFrags[currentId].emissive = emissivity;
			transparencyFrags[currentId].depth = gl_FragCoord.z;
			transparencyFrags[currentId].directLighting = vec3(0.);
			transparencyFrags[currentId].nextId = nextId;
		}

		discard;
	}
}