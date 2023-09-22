#version 450

#define PI				3.1415926535
#define TWO_PI			6.2831853071
#define PCF_SAMPLES     8.
#define PCF_OFFSET      0.1
#define EPS				0.0001

#define pow2(a) (a)*(a)

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPositionMap;
layout( binding = 1 ) uniform sampler2D uNormal_MetalnessMap;
layout( binding = 2 ) uniform sampler2D uAlbedo_RoughnessMap;
layout( binding = 3 ) uniform sampler2DShadow uShadowMap;

uniform vec4 uCamData;
uniform mat4 uLightMatrix_VP;
uniform vec3 uLightDirection;
uniform vec3 uLightEmissivity;

in vec2 uv;



/*float shadow_slope_bias_get(vec2 atlas_size, LightData light, vec3 lNg, vec3 lP, vec2 uv, uint lod)
{
  // Compute coordinate inside the pixel we are sampling.
  vec2 uv_subpixel_coord = fract(uv * atlas_size);
  // Bias uv sample for LODs since custom raster aligns LOD pixels instead of centering them.
  uv_subpixel_coord += (lod > 0) ? -exp2(-1.0 - float(lod)) : 0.0;
  // Compute delta to the texel center (where the sample is).
  vec2 ndc_texel_center_delta = uv_subpixel_coord * 2.0 - 1.0;
  // Create a normal plane equation and go through the normal projection matrix.
  vec4 lNg_plane = vec4(lNg, -dot(lNg, lP));
  vec4 ndc_Ng = shadow_load_normal_matrix(light) * lNg_plane;
  // Get slope from normal vector. Note that this is signed.
  vec2 ndc_slope = ndc_Ng.xy / abs(ndc_Ng.z);
  // Clamp out to avoid the bias going to infinity. Remember this is in NDC space.
  ndc_slope = clamp(ndc_slope, -100.0, 100.0);
  // Compute slope to where the receiver should be by extending the plane to the texel center.
  float bias = dot(ndc_slope, ndc_texel_center_delta);
  // Bias for 1 pixel of the sampled LOD.
  bias /= ((SHADOW_TILEMAP_RES * SHADOW_PAGE_RES) >> lod);
  return bias;
}*/





// ------------- fresnel -------------	:	schilk
float schlick(in float f0, in float f90, in float cosT){
    float tmp = 1.-cosT;
    return f0 + (f90 - f0) * tmp*tmp*tmp*tmp*tmp;
} 

vec3 schlick(in vec3 f0, in vec3 f90, in float cosT){ 
	float tmp = 1.-cosT;
	return f0 + (f90-f0) * tmp*tmp*tmp*tmp*tmp;
}

void main()
{
	vec4 position = texture(uPositionMap,uv);
	if(position.a<0.5) discard;

	vec4 albedo_roughness = texture(uAlbedo_RoughnessMap,uv);
	vec4 normal_metalness = texture(uNormal_MetalnessMap,uv);

	vec3 N = normal_metalness.xyz;
	vec3 V = normalize(uCamData.xyz-position.xyz);
	
	// ---------- LIGHT ----------
	vec3 L = -uLightDirection;
	
	float cosNL = dot(N,L);
	float cosNV = dot(N,V);
	/*if(cosNL<0. && cosNV<0.) {N*=-1.; cosNV=-1.; cosNL*=-1.;}
	if(cosNL<0. || cosNV<0.) discard;*/
	if(cosNL<0. && cosNV<0.) N=-N;
	cosNL = abs(cosNL);
	cosNV = abs(cosNV);

	// --- SHADOW ---
	vec3 T = cross(N,vec3(1.,0.,0.));
	if(length(T)<0.1) T = cross(N,vec3(0.,1.,0.));
	T = normalize(T);
	vec3 B = normalize(cross(N,T));

	// --- PCF => todo implement pcss
	float shadow  = 0.;
	for(float x = -PCF_OFFSET; x<PCF_OFFSET; x += PCF_OFFSET/(PCF_SAMPLES*0.5))
		for(float y = -PCF_OFFSET; y<PCF_OFFSET; y += PCF_OFFSET/(PCF_SAMPLES*0.5)){
				vec3 fp = position.xyz + T*x + B*y + L*clamp(0.05*(cosNL),0.05,0.1); // N better but bug black zones 
				//vec3 fp = position.xyz + T*x + B*y + N*EPS + L*max(0.5,cosNL)*1000./uCamData.a;
				vec3 fpLS = (uLightMatrix_VP * vec4(fp,1.)).xyz;
				shadow += (fpLS.z < 1.) ? texture(uShadowMap,fpLS) : 1.;
			}
	shadow /= (PCF_SAMPLES * PCF_SAMPLES);

	// ---------- SHADING ----------
	vec3 H = normalize(V+L);
	float cosHV = dot(H,V);
	float cosHL = dot(H,L);
	float cosHN = dot(H,N);

	// --- dielectic ---
	float f90 = albedo_roughness.a * (2.*cosHL*cosHL+0.5);
    vec3 dielectric = albedo_roughness.xyz * schlick(1.,f90,cosNL) * schlick(1.,f90,cosNV) * (1.-albedo_roughness.a*0.51/1.51) / PI;
	
	// --- conductor ---   
	float r = albedo_roughness.a * albedo_roughness.a;
    float r2 = r*r;
    
    // use fresnel normal incidence for m.ior and 1.
	//vec3 F0 = mix(vec3(0.04),albedo_roughness.xyz,normal_metalness.a);
	//vec3 F = schlick(F0.xyz,vec3(1.),cosHL);
    vec3 F = schlick(albedo_roughness.xyz,vec3(1.),cosHL);
    float D = r2/(PI*pow2(cosHN*cosHN*(r2-1.)+1.));
    float V2 = 0.5/(cosNL*sqrt((cosNV-cosNV*r2)*cosNV+r2) + cosNV*sqrt((cosNL-cosNL*r2)*cosNL+r2));
    vec3 conductor = F*D*V2;

	//fragColor = vec4(mix(dielectric,conductor,normal_metalness.a) * uLightEmissivity * shadow * cosNL,1.);
	//fragColor = vec4(((1.-F)*dielectric+conductor) * uLightEmissivity * shadow * cosNL,1.);
	fragColor = vec4( mix(mix(dielectric,conductor,schlick(0.04,1.,cosHL)),conductor,normal_metalness.a) * uLightEmissivity * shadow * cosNL,1.);
}