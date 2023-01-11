#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 normalMat;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in vec3 inKa;
layout(location = 6) in vec3 inKd;
layout(location = 7) in vec3 inKs;
layout(location = 8) in float inS;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragPosT;
layout(location = 2) out vec2 uv;
layout(location = 3) out vec3 normal;
layout(location = 4) out vec3 ka;
layout(location = 5) out vec3 kd;
layout(location = 6) out vec3 ks;
layout(location = 7) out float s;

void main() {
    uv = inUV;
    ka = inKa;
    kd = inKd;
    ks = inKs;
    s = inS;

    mat4 mv = ubo.view * ubo.model;

    vec3 N	 = normalize( (mv * vec4(inNormal,0.f)).xyz );
	vec3 T	 = normalize( (mv * vec4(inTangent,0.f)).xyz );
	vec3 B	 = normalize( (mv * vec4(inBitangent,0.f)).xyz );
	mat3 inv_TBN = inverse(mat3(T,B,N));

    normal = (ubo.normalMat * vec4(inNormal,1.f)).xyz;
    fragPos = (mv * vec4( inPosition, 1.f)).xyz;
    fragPosT = inv_TBN * fragPos;
    gl_Position = ubo.proj * mv * vec4( inPosition, 1.f);
}