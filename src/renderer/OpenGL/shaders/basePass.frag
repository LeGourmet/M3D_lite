#version 450

layout( location = 0 ) out vec4 position_metalness;
layout( location = 1 ) out vec4 normal_roughness;
layout( location = 2 ) out vec4 ambient;
layout( location = 3 ) out vec4 albedo;

layout( binding = 0 ) uniform sampler2D uAmbientMap;
layout( binding = 1 ) uniform sampler2D uAlbedoMap;
layout( binding = 2 ) uniform sampler2D uMetalnessMap;
layout( binding = 3 ) uniform sampler2D uRoughnessMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;

uniform vec3 uAmbient;
uniform vec3 uAlbedo;
uniform float uMetalness;
uniform float uRoughness;

uniform bool uHasAmbientMap;
uniform bool uHasAlbedoMap;
uniform bool uHasMetalnessMap;
uniform bool uHasRoughnessMap;
uniform bool uHasNormalMap;

in vec2 uv;
in vec3 fragNormal;
in vec3 fragPosition;
in mat3 TBN;

void main(){	
	// transparence ?
	// care clear color

	float metalness = (uHasMetalnessMap ? texture2D(uMetalnessMap,uv).x : uMetalness);
	position_metalness = vec4(fragPosition,metalness);

	float roughness = (uHasRoughnessMap ? texture2D(uRoughnessMap,uv).x : uRoughness);
	vec3 normal = normalize(uHasNormalMap ? (TBN*(texture2D(uNormalMap,uv).xyz*2.-1.)) : fragNormal);
	normal_roughness = vec4(normal,roughness);
	
	albedo = vec4((uHasAlbedoMap ? texture2D(uAlbedoMap,uv).xyz : uAlbedo),1.);
	ambient = vec4(0.01*(uHasAmbientMap ? texture2D(uAmbientMap,uv).xyz : uAmbient),1.);
}