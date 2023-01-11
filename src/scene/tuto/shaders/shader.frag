#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragPosT;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 ka;
layout(location = 5) in vec3 kd;
layout(location = 6) in vec3 ks;
layout(location = 7) in float s;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 Li = normalize(-fragPos);
	vec3 Lo = Li;					
	vec3 N = normalize(normal);		
	if(dot(N,Lo)<0){N *= -1.f;}

	vec3 H = normalize(Lo+Li);

	float Id = max(0.f,dot(N,Li));
	float Is = max(0.f,pow(dot(H, N), s ));

	vec3 colorL = vec3(1.f);
		
	outColor = vec4(ka + ( Id*kd + Is*ks) * colorL, 1.f)*max(0.f,dot(N,Li));
}