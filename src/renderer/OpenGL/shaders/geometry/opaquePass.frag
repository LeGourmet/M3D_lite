#version 450

layout( location = 0 ) out vec4 position;
layout( location = 1 ) out vec4 normal_metalness;
layout( location = 2 ) out vec4 albedo_roughness;
layout( location = 3 ) out vec4 emissive;

layout( binding = 1 ) uniform sampler2D uAlbedoMap;
layout( binding = 2 ) uniform sampler2D uMetalnessRoughnessMap;
layout( binding = 3 ) uniform sampler2D uNormalMap;
layout( binding = 4 ) uniform sampler2D uEmissiveMap;

layout( binding = 5, std430)	buffer uSSBOTransparency { float LinkedList[]; };
layout( binding = 6, r32ui)		uniform uimage2D uRootTransparency;
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

	if(uAlphaCutOff<1.){ // opaque
		if(albedo.a<uAlphaCutOff) discard;

		vec3 normal = normalize(uHasNormalMap ? (TBN*(texture(uNormalMap,uv).xyz*2.-1.)) : fragNormal);
		vec2 MetalnessRoughness = (uHasMetalnessRoughnessMap ? texture(uMetalnessRoughnessMap,uv).zy : vec2(uMetalness,uRoughness));
		vec3 emissive_componant = (uHasEmissiveMap ? texture(uEmissiveMap,uv).xyz : uEmissiveColor) * uEmissiveStrength;

		position = vec4(fragPosition,1.);
		normal_metalness = vec4(normal, MetalnessRoughness.x);
		albedo_roughness = vec4(albedo.xyz,MetalnessRoughness.y);
		emissive = vec4(emissive_componant,1.);
	}else{	// transparent
		uint currentId = atomicCounterIncrement(uCounterTransparency);

		if( currentId < uNbFragmentsMax ) {
			uint previousId = imageAtomicExchange(uRootTransparency, ivec2(gl_FragCoord.xy), currentId); // wtf ivec2 ???

			LinkedList[currentId*5  ] = albedo.x;
			LinkedList[currentId*5+1] = albedo.y;
			LinkedList[currentId*5+2] = albedo.z;
			LinkedList[currentId*5+3] = gl_FragCoord.z;
			LinkedList[currentId*5+4] = previousId;	
			// emissive + normal + metal + rough
		}

		discard;
	}
}