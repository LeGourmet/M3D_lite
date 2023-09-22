#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPositionMap;
layout( binding = 1 ) uniform sampler2D uNormal_MetalnessMap;
layout( binding = 2 ) uniform sampler2D uAlbedo_RoughnessMap;
layout( binding = 3 ) uniform sampler2D uDirectLightingMap; 

uniform vec3 uCamPos;

in vec2 uv;

void main()
{
	vec4 position = texture(uPositionMap,uv);
	if(position.a<0.5) discard;

	vec3 N = texture(uNormal_MetalnessMap,uv).xyz;

	// visibility bitmask initialisation 
	// sample position en direction de la normal N from radius(3) genre 16-32 fois
	// define angle en fonction de t(0.5 - 0.25) et de la normal
	// update bitmask 

	// moyenne de visible/total
	float occlusion = 1.;

	//vec3 V = normalize(uCamPos-position.xyz);
	//float cosNV = dot(N,V);

	// lancer de rayon au millieux de la section à travers direct lighting Map
	vec3 col = occlusion * vec3(0.01); // rayon * cos(N,L);
	fragColor = vec4(texture(uAlbedo_RoughnessMap,uv).xyz*col,1.);
}