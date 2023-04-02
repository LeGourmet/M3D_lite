#version 450

layout( location = 0 ) out vec4 position;
layout( location = 1 ) out vec4 normal_metalness;
layout( location = 2 ) out vec4 albedo_roughness;
layout( location = 3 ) out vec4 emissivity;

layout( binding = 1 ) uniform sampler2D uAlbedoMap;
layout( binding = 2 ) uniform sampler2D uMetalnessRoughnessMap;
layout( binding = 3 ) uniform sampler2D uNormalMap;
layout( binding = 4 ) uniform sampler2D uEmissivityMap;
// occlusion map

uniform vec3 uAlbedo;
uniform float uMetalness;
uniform float uRoughness;
uniform vec3 uEmissivity;

uniform bool uHasAlbedoMap;
uniform bool uHasMetalnessRoughnessMap;
uniform bool uHasNormalMap;
uniform bool uHasEmissivityMap;

in vec2 uv;
in vec3 fragNormal;
in vec3 fragPosition;
in mat3 TBN;

void main(){
	vec2 MetalnessRoughness = (uHasMetalnessRoughnessMap ? texture2D(uMetalnessRoughnessMap,uv).xy : vec2(uMetalness,uRoughness));
	vec3 normal = normalize(uHasNormalMap ? (TBN*(texture2D(uNormalMap,uv).xyz*2.-1.)) : fragNormal);

	position = vec4(fragPosition,1.);
	normal_metalness = vec4(normal,MetalnessRoughness.x);
	albedo_roughness = vec4((uHasAlbedoMap ? texture2D(uAlbedoMap,uv).xyz : uAlbedo),MetalnessRoughness.y);
	emissivity = vec4((uHasEmissivityMap ? texture2D(uEmissivityMap,uv).xyz : uEmissivity),1.);
}