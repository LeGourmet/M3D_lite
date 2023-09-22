#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 1 ) uniform sampler2D uAlbedoMap;
layout( binding = 2 ) uniform sampler2D uEmissiveMap;
layout( binding = 3 ) uniform sampler2DShadow uLayerMap;

uniform vec4 uAlbedo;
uniform vec3 uEmissiveColor;
uniform float uEmissiveStrength;
uniform float uAlphaCutOff;

uniform bool uHasAlbedoMap;
uniform bool uHasEmissiveMap;

in vec2 uv;

void main(){
	vec4 albedo = (uHasAlbedoMap ? texture(uAlbedoMap,uv) : vec4(1.))*uAlbedo;
	if(uAlphaCutOff<1.) discard;
	
	if(texture(uLayerMap,vec3(((gl_FragCoord.xy+1.)*0.5),1.)) <= (gl_FragCoord.z+1.)*0.5) discard;

	fragColor = albedo + vec4((uHasEmissiveMap ? texture(uEmissiveMap,uv).xyz : uEmissiveColor) * uEmissiveStrength,0.); 
}