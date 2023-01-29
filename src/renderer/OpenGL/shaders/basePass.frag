#version 450

layout( location = 0 ) out vec4 ambientColor;
layout( location = 1 ) out vec4 diffuseColor;
layout( location = 2 ) out vec4 specularColor;
layout( location = 3 ) out vec4 shininess;
layout( location = 4 ) out vec4 normal;
layout( location = 5 ) out vec4 position;

layout( binding = 0 ) uniform sampler2D uAmbientMap;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 2 ) uniform sampler2D uSpecularMap;
layout( binding = 3 ) uniform sampler2D uShininessMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;

uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;

uniform bool uHasAmbientMap;
uniform bool uHasDiffuseMap;
uniform bool uHasSpecularMap;
uniform bool uHasShininessMap;
uniform bool uHasNormalMap;

in vec2 uv;
in vec3 fragNormal;
in vec3 fragPosition;
in mat3 TBN;

void main(){	
	// transparence ?
	// care clear color

	//ambientColor = vec4(fragPosition,1.);
	ambientColor	= vec4(0.1*(uHasAmbientMap ? texture2D(uAmbientMap,uv).xyz : uAmbient),1.);
	diffuseColor	= vec4((uHasDiffuseMap ? texture2D(uDiffuseMap,uv).xyz : uDiffuse),1.);
	specularColor	= vec4((uHasSpecularMap ? texture2D(uSpecularMap,uv).xyz : uSpecular),1.);
	shininess		= vec4(uHasShininessMap ? texture2D(uShininessMap,uv).x : uShininess);
	normal			= vec4(normalize(uHasNormalMap ? (TBN*(texture2D(uNormalMap,uv).xyz*2.f-1.f)) : fragNormal),1.);
	position		= vec4(fragPosition,1.);
}