#version 450

layout( location = 0 ) out vec4 position_metalness;
layout( location = 1 ) out vec4 normal_roughness;
layout( location = 2 ) out vec4 albedo;

layout( binding = 1 ) uniform sampler2D uAlbedoMap;
layout( binding = 2 ) uniform sampler2D uMetalnessRoughnessMap;
layout( binding = 3 ) uniform sampler2D uNormalMap;
// occlusion map
// emissivity map

uniform vec3 uAlbedo;
uniform float uMetalness;
uniform float uRoughness;

uniform bool uHasAlbedoMap;
uniform bool uHasMetalnessRoughnessMap;
uniform bool uHasNormalMap;

in vec2 uv;
in vec3 fragNormal;
in vec3 fragPosition;
in mat3 TBN;

void main(){	
	// transparence ?
	// care clear color

	vec2 MetalnessRoughness = (uHasMetalnessRoughnessMap ? texture2D(uMetalnessRoughnessMap,uv).xy : vec2(uMetalness,uRoughness));
	vec3 normal = normalize(uHasNormalMap ? (TBN*(texture2D(uNormalMap,uv).xyz*2.-1.)) : fragNormal);

	position_metalness = vec4(fragPosition,MetalnessRoughness.x);
	normal_roughness = vec4(normal,MetalnessRoughness.y);
	albedo = vec4((uHasAlbedoMap ? texture2D(uAlbedoMap,uv).xyz : uAlbedo),1.);
}