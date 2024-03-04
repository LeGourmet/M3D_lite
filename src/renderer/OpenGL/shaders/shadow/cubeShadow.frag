#version 450

layout( binding = 1 ) uniform sampler2D uAlbedoMap;

uniform vec4 uAlbedo;
uniform bool uHasAlbedoMap;
uniform float uAlphaCutOff;

uniform vec3 uLightPos;
uniform float uZfar;

in vec4 fragPos;
in vec2 fragUVs;

void main()
{
    if(uAlphaCutOff<1.) {
		vec4 albedo = (uHasAlbedoMap ? texture(uAlbedoMap,fragUVs) : vec4(1.))*uAlbedo;
		if(albedo.a<uAlphaCutOff) discard;
	}
    gl_FragDepth = distance(fragPos.xyz,uLightPos)/uZfar;
} 