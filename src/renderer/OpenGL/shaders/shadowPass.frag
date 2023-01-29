#version 450

layout(location = 0) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uAmbientMap;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 2 ) uniform sampler2D uSpecularMap;
layout( binding = 3 ) uniform sampler2D uShininessMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;

uniform vec3 uCamPos;
//uniform vec4 uLightPosition;	 // position  + type
//uniform vec4 uLightDirection;	 // direction + angle Min (entre -1 et 1)
//uniform vec4 uLightEmissivity; // emission  + angle Max (entre -1 et 1)

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
in mat3 TBN;

void main()
{
	// transparence ?
	/*vec4 uLightPosition = vec4(0.);
	vec4 uLightDirection = vec4(0.,-0.5,-1.,0.);
	vec4 uLightEmissivity = vec4(vec3(1.),0.);*/
	/*vec4 uLightPosition = vec4(10., 20., 10.,1.);
	vec4 uLightDirection = vec4(-0.5,-0.5,-0.5,0.95);
	vec4 uLightEmissivity = vec4(vec3(500.),1.);*/
	vec4 uLightPosition = vec4(10., 20., 10.,1.);
	vec4 uLightDirection = vec4(-0.5,-0.5,-0.5,0.9);
	vec4 uLightEmissivity = vec4(vec3(500.),1.);

	vec3 Li,Light_Componant;
	if(uLightPosition.w <= 0.5){
		Li = normalize(-uLightDirection.xyz);
		Light_Componant = uLightEmissivity.xyz;
	}else{
		Li = uLightPosition.xyz-fragPos;
		float attenuation = 1./dot(Li,Li); // or 1./(a*d*d + b*d + c)
		Li = normalize(Li);
		float intensity = clamp((dot(-Li, normalize(uLightDirection.xyz)) - uLightDirection.w) / (uLightEmissivity.w-uLightDirection.w), 0., 1.);
		Light_Componant = uLightEmissivity.xyz * intensity * attenuation; 
	}
	
	vec3 Lo = normalize(uCamPos-fragPos);
	vec3 N = normalize(uHasNormalMap ? (TBN*(texture2D(uNormalMap,uv).xyz*2.f-1.f)) : normal);
	vec3 H = normalize(Lo+Li);
	//if(dot(N,Lo)<0.) N *= -1.;

	float cosT = max(0.,dot(N,Li));
	float shininess = (uHasShininessMap ? texture2D(uShininessMap,uv).x : uShininess);

	
	vec3 Ambiant_Componant  = 0.01f                                  * (uHasAmbientMap ? texture2D(uAmbientMap,uv).xyz : uAmbient);
	vec3 Diffuse_Componant  = 1.f*cosT                               * (uHasDiffuseMap ? texture2D(uDiffuseMap,uv).xyz : uDiffuse);
	vec3 Specular_Componant = 1.f*pow(max(0.f,dot(N,H)), shininess)  * (uHasSpecularMap ? texture2D(uSpecularMap,uv).xxx : uSpecular);

	fragColor = vec4(Ambiant_Componant+(Diffuse_Componant + Specular_Componant)*Light_Componant*cosT,1.);
	//fragColor = vec4(uHasNormalMap ? TBN*(texture2D(uNormalMap,uv).xyz*2.-1.) : vec3(1.),1.);
	//fragColor = vec4((uHasDiffuseMap ? texture2D(uDiffuseMap,uv).xyz : uDiffuse),1.);
}