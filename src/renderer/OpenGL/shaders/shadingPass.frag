#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPosition_MetalnessMap;
layout( binding = 1 ) uniform sampler2D uNormal_RoughnessMap;
layout( binding = 2 ) uniform sampler2D uAlbedoMap;

uniform vec3 uCamPos;
uniform vec4 uLightPosition;	 // position  + type
uniform vec4 uLightDirection;	 // direction + angle Min (entre -1 et 1)
uniform vec4 uLightEmissivity;   // emission  + angle Max (entre -1 et 1)
in vec2 uv;

void main()
{
	vec4 albedo = texture2D(uAlbedoMap,uv);
	if(albedo.a<0.5) discard;

	vec4 position_metalness = texture2D(uPosition_MetalnessMap,uv);
	vec4 normal_roughness = texture2D(uNormal_RoughnessMap,uv);

	vec3 L,Light_Componant;
	if(uLightPosition.w < 0.5){
		L = normalize(-uLightDirection.xyz);
		Light_Componant = uLightEmissivity.xyz;
	}else{
		L = uLightPosition.xyz-position_metalness.xyz;
		float attenuation = 1./dot(L,L); // or 1./(a*d*d + b*d + c)
		L = normalize(L);
		float intensity = clamp((dot(-L, normalize(uLightDirection.xyz)) - uLightDirection.w) / (uLightEmissivity.w-uLightDirection.w), 0., 1.);
		Light_Componant = uLightEmissivity.xyz * intensity * attenuation; 
	}

	vec3 N = normal_roughness.xyz;
	float cosNL = dot(N,L); // cti
	if(cosNL <= 0.) discard;

	vec3 V = normalize(uCamPos-position_metalness.xyz);
	float cosNV = dot(N,V); // cto
	if(cosNV <= 0.) discard;

	float PI = 3.14;

	float roughness = normal_roughness.a;//sqrt(2./(normal_roughness.a+2.));
	float r2 = roughness*roughness;
	vec3 H = normalize(V+L);
	float cosNH = dot(N,H);
	float cosHV = dot(H,V);

	// ---------- diffuse BRDF ----------	:	oren Nayar
	float To = acos(cosNV);
	float Ti = acos(cosNL);
	float PHI = dot(normalize(V-N*cosNL),normalize(L-N*cosNV));
	float A = 1.-0.5*(r2/(r2+0.33));
	float B	= 0.45*(r2/(r2+0.09));
	float diffuseBRDF = (A+( B * max(0.,PHI) * sin(max(Ti,To)) * tan(min(Ti,To)) ) ) / PI;	//==> oren Nayar

	// ---------- specular BRDF ----------	:	GGX
	float Fo = 0.04;				// F0 = lerp(F0dielectric,albedo,metalness);
	float tmpF = 1.- cosHV;
	float F = Fo + ( 1. - Fo ) * tmpF*tmpF*tmpF*tmpF*tmpF;
	
	float tmpG = 2.*cosNH/cosHV;
	float G = min(1.,min(tmpG*cosNV,tmpG*cosNL));

	float tmpD = (cosNH*cosNH)*(r2-1.)+1.;
	float D = r2/(PI*tmpD*tmpD);

	float specularBRDF =  F*G*D/(4.*cosNV*cosNL);

	fragColor = vec4(albedo.xyz * ((1.-position_metalness.a)*diffuseBRDF + position_metalness.a*specularBRDF) * Light_Componant * cosNL, 1.); 
}