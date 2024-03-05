#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPositionMap;
layout( binding = 1 ) uniform sampler2D uNormal_MetalnessMap;
layout( binding = 2 ) uniform sampler2D uAlbedo_RoughnessMap;
layout( binding = 3 ) uniform sampler2D uDirectLightingMap; 

uniform vec3 uCamPos;

in vec2 uv;

/*int CountBits(int val) {
    //Counts the number of 1:s //https://www.baeldung.com/cs/integer-bitcount
    val = (val&0x55555555)+((val>>1)&0x55555555);
    val = (val&0x33333333)+((val>>2)&0x33333333);
    val = (val&0x0F0F0F0F)+((val>>4)&0x0F0F0F0F);
    val = (val&0x00FF00FF)+((val>>8)&0x00FF00FF);
    val = (val&0x0000FFFF)+((val>>16)&0x0000FFFF);
    return val;
}*/

void main()
{
	/*vec4 position = texture(uPositionMap,uv);
	if(position.a<0.5) discard;

	vec3 N = texture(uNormal_MetalnessMap,uv).xyz;

	// visibility bitmask initialisation 
	// sample position en direction de la normal N from radius(3) genre 16-32 fois
	// define angle en fonction de t(0.5 - 0.25) et de la normal
	// update bitmask 

	// moyenne de visible/total
	float occlusion = 1.;

	//vec3 V = normalize(uCamPos-position.xyz);
	//float cosNV = dot(N,V);

	// lancer de rayon au millieux de la section � travers direct lighting Map
	vec3 col = occlusion * vec3(10.); // rayon * cos(N,L);
	fragColor = vec4(texture(uAlbedo_RoughnessMap,uv).xyz*col,1.);*/

	/*
    seed = uint(iFrame*(int(iResolution.x*iResolution.y)))+uint(fragCoord.x+fragCoord.y*iResolution.x);
    
    vec2 SSOffset = vec2(0.);
    vec3 Pos = texture(iChannel0,vec2(3.5,0.5)*IRES).xyz;
    vec3 Eye = texture(iChannel0,vec2(2.5,0.5)*IRES).xyz;
    vec3 Tan; vec3 Bit = TBN(Eye,Tan);
    mat3 EyeMat = TBN(Eye);
    vec3 Dir = normalize(vec3(((fragCoord+SSOffset)*IRES*2.-1.)*(ASPECT*CFOV),1.)*EyeMat);
    vec4 Attr = texture(iChannel0,fragCoord*IRES);
    if (Attr.y>-0.5) { // if !emissive nor sky 
        //Geometry
        vec3 PPos = Pos+Dir*Attr.w;
        vec3 Normal = FloatToVec3(Attr.z)*2.-1.;
        
        //Screen space horizons
        Output.xyz = vec3(0.);
        vec3 VNormal = vec3(dot(Normal,Tan),dot(Normal,Bit),dot(Normal,Eye));
        vec3 VPPos = vec3(dot(PPos-Pos,Tan),dot(PPos-Pos,Bit),dot(PPos-Pos,Eye));
        vec2 ModFC = mod(fragCoord,4.);
        float RandPhiOffset = rand();//hash(vec2(1.234)+mod(CurrentFrame*3.26346,7.2634));
        float RandPhi = rand();//(mod(floor(ModFC.x)+floor(ModFC.y)*4.+CurrentFrame*5.,16.)+RandPhiOffset)*2.*PI*I64;
        for (float i=0.; i<3.5; i++) {
            //4 hemi-horizons
            RandPhi += PI*0.5;
            vec2 SSDir = vec2(cos(RandPhi),sin(RandPhi));
            float StepDist = 1.;
            float StepCoeff = 0.15+0.15*rand();//hash(fragCoord*IRES*(1.4+mod(float(iFrame)*3.26346,6.2634)));
            int BitMask = int(0);
            for (float s=1.; s<32.5; s++) {
                //32 steps
                vec2 SUV = fragCoord+SSDir*StepDist;
                float CurrentStep = max(1.,StepDist*StepCoeff);
                StepDist += CurrentStep;
                if (DFBox(SUV-1.,RES-1.)>0.) break;
                vec4 SAttr = texture(iChannel0,SUV*IRES);
                if (SAttr.y<-1.5) continue;
                vec3 SVPPos = normalize(vec3((SUV*IRES*2.-1.)*(ASPECT*CFOV),1.))*SAttr.w;
                float NorDot = dot(VNormal,SVPPos-VPPos)-0.001;
                float TanDist = length(SVPPos-VPPos-NorDot*VNormal);
                float Angle1f = atan(NorDot,TanDist);
                float Angle2f = atan(NorDot-0.03*max(1.,StepDist*0.07),TanDist);
                float Angle1 = max(0.,ceil(Angle1f/(PI*0.5)*32.));
                float Angle2 = max(0.,floor(Angle2f/(PI*0.5)*32.));
                //Sample bitmask
                int SBitMask = (int(pow(2.,Angle1-Angle2))-1) << int(Angle2);
                vec3 SNormal = FloatToVec3(SAttr.z)*2.-1.;
                SNormal = vec3(dot(SNormal,Tan),dot(SNormal,Bit),dot(SNormal,Eye));
                Output.xyz += float(CountBits(SBitMask & (~BitMask)))/max(1.,Angle1-Angle2)*FloatToVec3(SAttr.x)*LightCoeff
                              *(pow(cos(Angle2*I64*PI),2.)-pow(cos(Angle1*I64*PI),2.))
                              *sqrt(max(0.,dot(SNormal,-normalize(SVPPos-VPPos))));
                //Update bitmask
                BitMask = BitMask | SBitMask;
            }
        }
    } else { Output = vec4(FloatToVec3(Attr.x)*LightCoeff,0.); }
    fragColor = Output;	*/
}