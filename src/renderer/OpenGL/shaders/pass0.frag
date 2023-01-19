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

in vec2 uv;
in vec3 normal;
in vec3 fragPos;
in vec3 camPos;

void main()
{
	vec4 diffuseColor = (uHasDiffuseMap ? texture2D(uDiffuseMap,uv) : vec4(uDiffuse,1.f));
	if(diffuseColor.w<0.5f) discard;

	vec3 Li = normalize(camPos-fragPos);
	vec3 Lo = normalize(camPos-fragPos);
	vec3 N = normalize(normal);
	vec3 H = normalize(Lo+Li);
	if(dot(N,Lo)<0) N *= -1.f;

	vec3 Light = 1.f * vec3(1.f);
	float cosT = max(0.f,dot(N,Li));
	float shininess = (uHasShininessMap ? texture2D(uShininessMap,uv).x : uShininess);

	vec3 Ambiant_Componant  = 1.f                                   * (uHasAmbientMap ? texture2D(uAmbientMap,uv).xyz : uAmbient);
	vec3 Diffuse_Componant  = 1.f*cosT                              * diffuseColor.xyz;
	vec3 Specular_Componant = 1.f*pow(max(0.f,dot(N,H)), shininess) * (uHasSpecularMap ? texture2D(uSpecularMap,uv).xxx : uSpecular);

	fragColor = vec4(Ambiant_Componant+(Diffuse_Componant + Specular_Componant)*Light*cosT, diffuseColor.w);
}
