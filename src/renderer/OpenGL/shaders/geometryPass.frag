#version 450

layout( location = 0 ) out vec4 position;
layout( location = 1 ) out vec4 normal_metalness;
layout( location = 2 ) out vec4 albedo_roughness;
layout( location = 3 ) out vec4 emissive_ambient;

layout( binding = 1 ) uniform sampler2D uAlbedoMap;
layout( binding = 2 ) uniform sampler2D uMetalnessRoughnessMap;
layout( binding = 3 ) uniform sampler2D uNormalMap;
layout( binding = 4 ) uniform sampler2D uEmissiveMap;
layout (binding = 5 ) uniform sampler2D uOcclusionMap; 

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
uniform bool uHasOcclusionMap;

in vec2 uv;
in vec3 fragNormal;
in vec3 fragPosition;
in mat3 TBN;

void main(){
	vec4 albedo = (uHasAlbedoMap ? texture2D(uAlbedoMap,uv) : vec4(1.))*uAlbedo;
	
	if(albedo.a<uAlphaCutOff) discard;

	vec3 normal = normalize(uHasNormalMap ? (TBN*(texture2D(uNormalMap,uv).xyz*2.-1.)) : fragNormal);
	vec2 MetalnessRoughness = (uHasMetalnessRoughnessMap ? texture2D(uMetalnessRoughnessMap,uv).zy : vec2(uMetalness,uRoughness));
	vec3 emissivity = (uHasEmissiveMap ? texture2D(uEmissiveMap,uv).xyz : uEmissiveColor) * uEmissiveStrength;
	vec3 ambient = vec3(0.025)*albedo.xyz*(uHasOcclusionMap ? texture2D(uOcclusionMap,uv).x : 1.);

	position = vec4(fragPosition,1.);
	normal_metalness = vec4(normal, MetalnessRoughness.x);
	albedo_roughness = vec4(albedo.xyz,MetalnessRoughness.y);
	emissive_ambient = vec4(emissivity+ambient,1.);
}