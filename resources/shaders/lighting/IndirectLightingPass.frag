#version 460

#define PI_TWO 1.5707963267
#define PI     3.1415926535
#define TWO_PI 6.2831853071

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uLightingMap;
layout( binding = 1 ) uniform sampler2D uOpaqueAlbedoMap;
layout( binding = 2 ) uniform sampler2D uOpaqueNormalMap;
layout( binding = 3 ) uniform sampler2D uOpaqueMetalnessRoughnessMap;
layout( binding = 4 ) uniform sampler2D uOpaqueDepthMap;

uniform vec3 uCamPos;
uniform vec3 uCamDir;
uniform vec2 uScreenResolution;
uniform mat4 uInvMatrix_VP;

in vec2 uv;

uint seed; 

uint pcg(){
    uint state = seed*747796405U+2891336453U;
    uint tmp  = ((state >> ((state >> 28U) + 4U)) ^ state)*277803737U;
    return (seed = (tmp >> 22U) ^ tmp);
}

float rand(){return float(pcg())/float(0xffffffffU);}

vec3 computeFragPos(in vec2 p_uv){
    vec4 P = uInvMatrix_VP*vec4(2.*vec3(uv,texture(uOpaqueDepthMap,uv).x)-1.,1.);
    if (abs(P.a)>1e-5)  P /= P.a;
    return P.xyz;
}

/*vec3 sampleGGXVNDF(in vec3 V, in vec3 N, float rx, float ry) {
    float s = (N.z>=0.) ? 1. : -1.;
	float a = -1./(s+N.z);
	float b = N.x*N.y*a;
	vec3 T = vec3(1.+s*N.x*N.x*a, s*b, -s*N.x);
	vec3 B = vec3(b,s+N.y*N.y*a, -N.y);

    V = vec3(dot(V,T), dot(V,B), dot(V,N));
    vec3 Vh = normalize(V*vec3(rx,ry,1.));

    float phi = TWO_PI*rand();
    float z = (1.-rand())*(1.+Vh.z)-Vh.z;
    float sinTheta = sqrt(clamp(1.-z*z,0.,1.));
    vec3 Nh = normalize(vec3(sinTheta*cos(phi),sinTheta*sin(phi),z)+Vh);
    vec3 Ne = normalize(Nh*vec3(rx,ry,1.));

    return normalize(Ne.x*T + Ne.y*B + Ne.z*N);
}

vec3 sampleHemisphere(in vec3 N) {
    float a = TWO_PI * rand();
    float b = 2.*rand()-1.;
    return normalize(N+vec3(sqrt(1.-b*b)*vec2(cos(a),sin(a)), b));
}*/

int CountBits(int val) {
    val = (val&0x55555555)+((val>>1)&0x55555555);
    val = (val&0x33333333)+((val>>2)&0x33333333);
    val = (val&0x0F0F0F0F)+((val>>4)&0x0F0F0F0F);
    val = (val&0x00FF00FF)+((val>>8)&0x00FF00FF);
    val = (val&0x0000FFFF)+((val>>16)&0x0000FFFF);
    return val;
}

void main() {    
    fragColor = vec4(0.,0.,0.,1.); return;
    // sould be iFrame and not 0
    seed = uint(0*(int(uScreenResolution.x*uScreenResolution.y)))+uint(uv.x*uScreenResolution.x+uv.y*uScreenResolution.y*uScreenResolution.x);

    vec3 N = texture(uOpaqueNormalMap,uv).xyz;
	if(N.x==0. && N.y==0. && N.z==0.) discard;
    vec3 P = computeFragPos(uv);
    
    // we can use cam right, left, up, down, ... 
    vec3 eye = uCamDir;
    vec3 eyeTan = (abs(eye.y)>0.999) ? vec3(1.,0.,0.) : normalize(cross(eye,vec3(0.,1.,0.)));
    vec3 eyeBit = (abs(eye.y)>0.999) ? vec3(0.,0.,1.) : normalize(cross(eyeTan,eye));        
    
    //vec3 V = normalize(uCamPos-P);

    vec3 VN = normalize(vec3(dot(N,eyeTan),dot(N,eyeBit),dot(N,eye)));
    vec3 VP = vec3(dot(P,eyeTan),dot(P,eyeBit),dot(P,eye));             // vec3(dot(PPos-Pos,Tan),dot(PPos-Pos,Bit),dot(PPos-Pos,Eye));
    
    /*vec2 metalnessRoughness = texture(uOpaqueMetalnessRoughnessMap,uv).xy;
    float m = metalnessRoughness.x;
    float r = clamp(metalnessRoughness.y, 0.04, 0.999);
          r = r*r;*/

    float RandPhi = (mod(floor(mod(uv.x*uScreenResolution.x,4.))+floor(mod(uv.y*uScreenResolution.y,4.))*4.,16.)+rand())*PI/32.;
       
    for(int i=0; i<4 ;i++){
        /*vec3 H = sampleGGXVNDF(V,N,r,r);    // random ? Z up ?
        float specularRate = 0.04 + (1.-0.04) * pow(max(0.,dot(H,V)),5);
        vec3 L = (rand()<specularRate/(specularRate+1.-m)) ? normalize(reflect(eye,H)) : sampleHemisphere(N); // random ? Z up ?
        vec3 VL = vec3(dot(L,eyeTan),dot(L,eyeBit),dot(L,eye));
        vec2 SSVL = normalize(VL.xy);                            // todo projete VL sur l'écran*/

       RandPhi += PI*0.5;
       vec2 SSVL = vec2(cos(RandPhi),sin(RandPhi));

        float StepDist = 1.;
        float StepCoeff = 0.15+0.15*rand();
              
        int BitMask = 0;
              
        for(int j=0; j<32 ;j++){
            vec2 FragUV = uv + SSVL*StepDist/uScreenResolution;
            StepDist += max(1.,StepDist*StepCoeff);
            
            if(FragUV.x<0. || FragUV.x>1. || FragUV.y<0. || FragUV.y>1.) break;

            vec3 FragN = texture(uOpaqueNormalMap,FragUV).xyz;
	        if(FragN.x==0. && FragN.y==0. && FragN.z==0.) continue;
            vec3 FragP = computeFragPos(FragUV);

            vec3 FragVP = vec3(dot(FragP,eyeTan),dot(FragP,eyeBit),dot(FragP,eye));             // normalize(vec3((FragUV*2.-1.)*(ASPECT*CFOV),1.))*FragSAttr.w;
            vec3 FragVN = normalize(vec3(dot(FragN,eyeTan),dot(FragN,eyeBit),dot(FragN,eye)));
            
            vec3 TRUC_CHELOU = FragVP-VP; // rename quand compris ce que c'est

            float NorDot = dot(VN,TRUC_CHELOU);
            if(NorDot<0.) continue; // utile ?
            float TanDist = length(TRUC_CHELOU-NorDot*VN);
            
            float AngleMax = max(0.,ceil(atan(NorDot,TanDist)/PI_TWO*32.));                             // sample touche the sector 
            float AngleMin = max(0.,floor(atan(NorDot-0.03*max(1.,StepDist*0.07),TanDist)/PI_TWO*32.)); // sample covers the entire sector
            
            int FragBitMask = (int(pow(2.,AngleMax-AngleMin))-1) << int(AngleMin);
            
            fragColor.xyz += float(CountBits(FragBitMask & (~BitMask)))/max(1.,AngleMax-AngleMin)*texture(uLightingMap,FragUV).xyz*texture(uOpaqueAlbedoMap,uv).xyz
                          *(pow(cos(AngleMin*PI/64.),2.)-pow(cos(AngleMax*PI/64.),2.))
                          *sqrt(max(0.,dot(FragVN,-normalize(TRUC_CHELOU))));
            
            BitMask = BitMask | FragBitMask;
        }
        
    }
}