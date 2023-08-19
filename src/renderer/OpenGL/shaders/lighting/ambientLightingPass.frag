#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPositionMap;
layout( binding = 1 ) uniform sampler2D uNormal_MetalnessMap;
layout( binding = 2 ) uniform sampler2D uAlbedo_RoughnessMap;

uniform vec3 uCamPos;

in vec2 uv;

void main()
{
	vec4 position = texture2D(uPositionMap,uv);
	if(position.a<0.5) discard;

	vec4 albedo_roughness = texture2D(uAlbedo_RoughnessMap,uv);
	vec4 normal_metalness = texture2D(uNormal_MetalnessMap,uv);

	vec3 N = normal_metalness.xyz;
	vec3 V = normalize(uCamPos-position.xyz);
	float cosNV = dot(N,V);

	fragColor = vec4(albedo_roughness.xyz*vec3(0.01),1.);}