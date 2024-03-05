#version 450

struct TranspFragNode {
    vec4 albedo;
    vec3 position;
    vec3 normal;
    vec3 emissive;
	float metalness;
    float roughness;
    float depth;
    uint nextId;
};

layout( location = 0 ) out vec4 opaquePosition;
layout( location = 1 ) out vec4 opaqueNormalMetalness;
layout( location = 2 ) out vec4 opaqueAlbedoRoughness;
layout( location = 3 ) out vec4 opaqueEmissive;

layout( binding = 1 )			uniform sampler2D uAlbedoMap;
layout( binding = 2 )			uniform sampler2D uMetalnessRoughnessMap;
layout( binding = 3 )			uniform sampler2D uNormalMap;
layout( binding = 4 )			uniform sampler2D uEmissiveMap;
layout( binding = 5, r32ui)		uniform uimage2D uRootTransparency;
layout( binding = 6, std430)	buffer uLinkedListTransparencyFrags { TranspFragNode transparencyFrags[]; };
layout( binding = 7, offset=0)	uniform atomic_uint uCounterTransparency;

uniform vec4 uAlbedo;
uniform vec3 uEmissiveColor;
uniform float uEmissiveStrength;
uniform float uMetalness;
uniform float uRoughness;
uniform float uAlphaCutOff;

uniform bool uHasAlbedoMap;
uniform bool uHasMetalnessRoughnessMap;
uniform bool uHasNormalMap;
uniform bool uHasEmissiveMap;

uniform uint uNbTranspFragsMax;

in vec2 uv;
in vec3 fragNormal;
in vec3 fragPosition;
in mat3 TBN;

void main(){
	vec4 albedo = (uHasAlbedoMap ? texture(uAlbedoMap,uv) : vec4(1.))*uAlbedo;

	vec3 normal = normalize(uHasNormalMap ? (TBN*(texture(uNormalMap,uv).xyz*2.-1.)) : fragNormal);
	vec2 MetalnessRoughness = (uHasMetalnessRoughnessMap ? texture(uMetalnessRoughnessMap,uv).zy : vec2(uMetalness,uRoughness));
	vec3 emissivity = (uHasEmissiveMap ? texture(uEmissiveMap,uv).xyz : uEmissiveColor) * uEmissiveStrength;
	
	if(uAlphaCutOff<1.){ // opaque
		if(albedo.a<uAlphaCutOff) discard;

		opaquePosition = vec4(fragPosition,1.);
		opaqueNormalMetalness = vec4(normal, MetalnessRoughness.x);
		opaqueAlbedoRoughness = vec4(albedo.xyz,MetalnessRoughness.y);
		opaqueEmissive = vec4(emissivity,1.);
	}else{	// transparent
		uint currentId = atomicCounterIncrement(uCounterTransparency);

		if( currentId < uNbTranspFragsMax ) {
			uint nextId = imageAtomicExchange(uRootTransparency, ivec2(gl_FragCoord.xy), currentId);
			transparencyFrags[currentId].albedo = albedo;
			transparencyFrags[currentId].position = fragPosition;
			transparencyFrags[currentId].normal = normal;
			transparencyFrags[currentId].emissive = emissivity;
			transparencyFrags[currentId].metalness = MetalnessRoughness.x;
			transparencyFrags[currentId].roughness = MetalnessRoughness.y;
			transparencyFrags[currentId].depth = gl_FragCoord.z;
			transparencyFrags[currentId].nextId = nextId;
		}

		discard;
	}
}