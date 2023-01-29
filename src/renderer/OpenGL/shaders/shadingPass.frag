#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uAmbientMap;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 2 ) uniform sampler2D uSpecularMap;
layout( binding = 3 ) uniform sampler2D uShininessMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;
layout( binding = 5 ) uniform sampler2D uPositionMap;

uniform vec3 uCamPos;
uniform vec4 uLightPosition;	 // position  + type
uniform vec4 uLightDirection;	 // direction + angle Min (entre -1 et 1)
uniform vec4 uLightEmissivity;   // emission  + angle Max (entre -1 et 1)
in vec2 uv;

void main()
{
	vec3 fragPos = texture2D(uPositionMap,uv).xyz;
	vec3 N		 = texture2D(uNormalMap,uv).xyz;

	vec3 Li,Light_Componant;
	if(uLightPosition.w < 0.5){
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
	vec3 H = normalize(Lo+Li);
	if(dot(N,Lo)<0.) N *= -1.;

	float cosT = max(0.,dot(N,Li));

	vec3 Diffuse_Componant  = cosT													* texture2D(uDiffuseMap,uv).xyz;
	vec3 Specular_Componant = pow(max(0.f,dot(N,H)), texture2D(uShininessMap,uv).x) * texture2D(uSpecularMap,uv).xyz;

	fragColor = vec4(texture2D(uAmbientMap,uv).xyz+(Diffuse_Componant + Specular_Componant)*Light_Componant*cosT,1.);
}