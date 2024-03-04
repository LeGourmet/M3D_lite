#version 450

layout( binding = 1 ) uniform sampler2D uAlbedoMap;

uniform vec4 uAlbedo;
uniform bool uHasAlbedoMap;
uniform float uAlphaCutOff;

in vec2 uv;

void main()
{
	if(uAlphaCutOff<1.) {
		vec4 albedo = (uHasAlbedoMap ? texture(uAlbedoMap,uv) : vec4(1.))*uAlbedo;
		if(albedo.a<uAlphaCutOff) discard;
	}
}