#version 450

#define PI				3.1415926535
#define TWO_PI			6.2831853071
#define INV_SQRT_TWO	0.7071067811
#define EPS				0.0001

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPositionMap;
layout( binding = 1 ) uniform sampler2D uNormal_MetalnessMap;
layout( binding = 2 ) uniform sampler2D uAlbedo_RoughnessMap;
layout( binding = 3 ) uniform sampler2DShadow uShadowMap;

uniform vec3 uCamPos;
uniform mat4 uLightMatrix_VP;
uniform vec3 uLightDirection;
uniform vec3 uLightEmissivity;

in vec2 uv;

// ------------- fresnel -------------	:	schilk
float getFresnel(float f0, float cosHV){ 
	float tmp = 1.-cosHV;
	return f0 + (1 - f0) * tmp*tmp*tmp*tmp*tmp;
}

vec3 getFresnel(vec3 f0, float cosHV){ return vec3(getFresnel(f0.x,cosHV),getFresnel(f0.y,cosHV),getFresnel(f0.z,cosHV)); }

// ---------- specular BRDF ----------	:	GGX+smith
float getSpecular(float a2, float cosNV, float cosNL, float cosNH){
	a2 *= a2; // re-maping

	// *** G : smith ***
    float tmpG = 1.-a2;
    float G = 2.*cosNV*cosNL/ (cosNL * sqrt(a2+tmpG*cosNV*cosNV) + cosNV * sqrt(a2+tmpG*cosNL*cosNL)); // care 0

	// *** D : GGX ***
	float tmpD = cosNH*cosNH*(a2-1.)+1.;
	float D = a2/(PI*tmpD*tmpD);		// care == 0

	return G*D/(4.*cosNV*cosNL);
}

// ---------- diffuse BRDF ----------	:	oren Nayar
float getDiffuse(float a, float cosNV, float cosNL, vec3 N, vec3 V, vec3 L ){
	float r_OrenNayar = atan(a)*INV_SQRT_TWO;
	float r2_OrenNayar = r_OrenNayar*r_OrenNayar;
	float To = acos(cosNV);
	float Ti = acos(cosNL);
	float PHI = dot(normalize(V-N*cosNL),normalize(L-N*cosNV));
	float A = 1.-0.5*(r2_OrenNayar/(r2_OrenNayar+0.33));
	float B	= 0.45*(r2_OrenNayar/(r2_OrenNayar+0.09));
	return (A+( B * max(0.,PHI) * sin(max(Ti,To)) * tan(min(Ti,To)) ) ) / PI;
}

void main()
{
	vec4 position = texture2D(uPositionMap,uv);
	if(position.a<0.5) discard;

	vec4 albedo_roughness = texture2D(uAlbedo_RoughnessMap,uv);
	vec4 normal_metalness = texture2D(uNormal_MetalnessMap,uv);

	vec3 N = normal_metalness.xyz;
	vec3 V = normalize(uCamPos-position.xyz);
	float cosNV = dot(N,V);
	if(cosNV<0.) { N *= -1.; cosNV *= -1.; }

	// ---------- LIGHT ----------
	vec3 L = -uLightDirection;
	float cosNL = dot(N,L);
	if(cosNL<0.) discard;

	// --- SHADOW ---
	vec3 fragPosLightSpace = (uLightMatrix_VP * vec4(position.xyz,1.)).xyz;
	fragPosLightSpace.z -= clamp(0.005*(1.-cosNL),0.0005,0.01); // compute better bias
	float shadow = (fragPosLightSpace.z < 1.) ? texture(uShadowMap,fragPosLightSpace) : 1.;

	/*pcf
	float shadow  = 0.0;
	float bias    = 0.05; 
	float samples = 4.0;
	float offset  = 0.1;
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			for(float z = -offset; z < offset; z += offset / (samples * 0.5))
			{
				float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; 
				closestDepth *= far_plane;   // undo mapping [0;1]
				if(currentDepth - bias > closestDepth)
					shadow += 1.0;
			}
		}
	}
	shadow /= (samples * samples * samples);
	*/

	// ---------- SHADING ----------
	vec3 H = normalize(V+L);
	float cosNH = dot(N,H);
	float cosHV = dot(H,V);

	float specular = getSpecular(albedo_roughness.a*albedo_roughness.a,cosNV,cosNL,cosNH);
	float diffuse = getDiffuse(albedo_roughness.a,cosNV,cosNL,N,V,L);

	vec3 dielectricComponent = albedo_roughness.xyz * mix(diffuse,specular,getFresnel(0.04,cosHV)); //we can use ior with ((1-ior)/(1+ior))^2 that emplace 0.04
	vec3 MetalComponent = getFresnel(albedo_roughness.xyz,cosHV) * specular;

	fragColor = vec4(mix(dielectricComponent,MetalComponent,normal_metalness.a) * uLightEmissivity * shadow * cosNL, 1.); 
}