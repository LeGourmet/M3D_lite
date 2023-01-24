#version 450

layout( location = 0 ) out vec4 fragColor;
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

in vec3 normal;
in vec3 fragPos;
in vec3 camPos;

void main()
{
	vec3 Li = normalize(camPos-fragPos);
	vec3 Lo = normalize(camPos-fragPos);
	vec3 N = normalize(normal);
	vec3 H = normalize(Lo+Li);
	if(dot(N,Lo)<0) N *= -1.f;

	float cosT = max(0.,dot(N,Li));

	vec3 Light_Componant = 1.f * vec3(1.f);
	vec3 Ambiant_Componant  = 1.f                                   * uAmbient;
	vec3 Diffuse_Componant  = 1.f*cosT                              * uDiffuse;
	vec3 Specular_Componant = 1.f*pow(max(0.f,dot(N,H)), uShininess) * uSpecular;

	fragColor = vec4(Ambiant_Componant+(Diffuse_Componant + Specular_Componant)*Light_Componant*cosT, 1.);
}