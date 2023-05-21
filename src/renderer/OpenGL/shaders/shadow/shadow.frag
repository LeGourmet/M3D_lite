#version 450

layout( binding = 1 ) uniform sampler2D uAlbedoMap;

uniform vec4 uAlbedo;
uniform bool uHasAlbedoMap;
uniform float uAlphaCutOff;

in vec2 uv;

void main()
{
	if((uHasAlbedoMap ? texture2D(uAlbedoMap,uv) : uAlbedo).a<uAlphaCutOff) discard;
}