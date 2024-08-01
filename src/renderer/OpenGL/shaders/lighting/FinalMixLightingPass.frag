#version 460

struct TranspFragNode {
    vec4 albedo;
    vec3 normal;
	float metalness;
    float roughness;
    vec3 emissive;
	vec3 directLighting;
    float depth;
    uint nextId;
};

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 )           uniform sampler2D uLightingMap;
layout( binding = 1 )           uniform sampler2D uIndirectLightingMap;
layout( binding = 2 )           uniform sampler2D uOpaqueNormalMap;
layout( binding = 3, r32ui )	uniform uimage2D uRootTransparency;
layout( binding = 4, std430 )	buffer uLinkedListTransparencyFrags { TranspFragNode transparencyFrags[]; };

uniform vec3 uCamPos;
uniform mat4 uInvMatrix_VP;

in vec2 uv;

void main(){
    vec3 rayColor = vec3(1.);
    vec3 finalColor = vec3(0.);

    for(uint i=imageLoad(uRootTransparency, ivec2(gl_FragCoord.xy)).r ;i!=0; i=transparencyFrags[i].nextId){
        finalColor += transparencyFrags[i].emissive*rayColor;
        finalColor += transparencyFrags[i].directLighting*rayColor;
        
        vec4 fragPos = uInvMatrix_VP*vec4(2.*vec3(uv,transparencyFrags[i].depth)-1.,1.);
        if (abs(fragPos.a)>1e-5)  fragPos /= fragPos.a;

        float r  = clamp(transparencyFrags[i].roughness, 0.04, 0.999);
		      r  = r*r;
	    float r2 = r*r;

        vec3 V = normalize(uCamPos-fragPos.xyz);
	    float cosNV = max(1e-5,abs(dot(transparencyFrags[i].normal,V)));

	    rayColor *= (1.-transparencyFrags[i].albedo.a) * sqrt(transparencyFrags[i].albedo.xyz);// * (1.-(0.04+(1.-0.04)*pow(1.-cosNV,5))) / sqrt(cosNV*cosNV*(1.-r2)+r2);

        // --- transmit ---
	    /* float D = r2/max(1e-10,(PI*pow2(cosHN*cosHN*(r2-1.)+1.)));
	       float V2 = 2./(2.*X*sqrt(X*X*(1.-r2)+r2) ));
	       vec3 transmit = sqrt(albedo) * (1.-DielF) * D * V2 * cosHL * cosHV / pow2(cosHV+cosHL); */
    }
    
    vec3 N = texture(uOpaqueNormalMap,uv).xyz;
	if(N.x==0. && N.y==0. && N.z==0.) finalColor += vec3(0.1)*rayColor; //skybox

    finalColor += (texture(uLightingMap,uv).xyz+texture(uIndirectLightingMap,uv).xyz)*rayColor;
    fragColor = vec4(finalColor,1.);
}