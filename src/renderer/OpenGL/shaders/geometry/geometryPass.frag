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

layout( location = 0 ) out vec4 position;
layout( location = 1 ) out vec4 normal_metalness;
layout( location = 2 ) out vec4 albedo_roughness;
layout( location = 3 ) out vec4 emissive;

layout( binding = 1 ) uniform sampler2D uAlbedoMap;
layout( binding = 2 ) uniform sampler2D uMetalnessRoughnessMap;
layout( binding = 3 ) uniform sampler2D uNormalMap;
layout( binding = 4 ) uniform sampler2D uEmissiveMap;

layout( binding = 5, r32ui)		uniform uimage2D uRootTransparency;
layout( binding = 6, std430)	buffer uLinkedListTransparency { FragNode nodes[]; };
layout( binding = 7, offset=0)	uniform atomic_uint uCounterTransparency;
uniform uint uNbFragmentsMax;

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

in vec2 uv;
in vec3 fragNormal;
in vec3 fragPosition;
in mat3 TBN;

void main(){
	vec4 albedo = (uHasAlbedoMap ? texture(uAlbedoMap,uv) : vec4(1.))*uAlbedo;

	vec3 normal = normalize(uHasNormalMap ? (TBN*(texture(uNormalMap,uv).xyz*2.-1.)) : fragNormal);
	vec2 MetalnessRoughness = (uHasMetalnessRoughnessMap ? texture(uMetalnessRoughnessMap,uv).zy : vec2(uMetalness,uRoughness));
	vec3 emissive_componant = (uHasEmissiveMap ? texture(uEmissiveMap,uv).xyz : uEmissiveColor) * uEmissiveStrength;


	if(uAlphaCutOff<1.){ // opaque
		if(albedo.a<uAlphaCutOff) discard;

		position = vec4(fragPosition,1.);
		normal_metalness = vec4(normal, MetalnessRoughness.x);
		albedo_roughness = vec4(albedo.xyz,MetalnessRoughness.y);
		emissive = vec4(emissive_componant,1.);
	}else{	// transparent
		uint currentId = atomicCounterIncrement(uCounterTransparency);

		if( currentId < uNbFragmentsMax ) {
			uint nextId = imageAtomicExchange(uRootTransparency, ivec2(gl_FragCoord.xy), currentId);
			nodes[currentId].albedo = albedo;
			nodes[currentId].position = fragPosition;
			nodes[currentId].normal = normal;
			nodes[currentId].emissive = emissive_componant;
			nodes[currentId].roughness = MetalnessRoughness.y;
			nodes[currentId].depth = gl_FragCoord.z;
			nodes[currentId].nextId = nextId;
		}

		discard;
	}
}