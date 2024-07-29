#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

layout(std430, binding = 0) buffer aTransformations { mat4 data_SSBO[]; };

out vec2 uv;
out vec3 fragNormal;
out mat3 TBN;

void main()
{
	mat4 Matrix_M = data_SSBO[gl_InstanceID*3];
	mat4 Matrix_MVP = data_SSBO[gl_InstanceID*3+1];
	mat4 Matrix_Normal = data_SSBO[gl_InstanceID*3+2];

	uv = aVertexTexCoords;

	fragNormal = (Matrix_Normal * vec4(aVertexNormal,0.)).xyz;

	vec3 N	 = normalize( (Matrix_M * vec4(aVertexNormal,0.)).xyz );
	vec3 T	 = normalize( (Matrix_M * vec4(aVertexTangent,0.)).xyz );
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(N, T));
	//vec3 B	 = normalize( (Matrix_M * vec4(aVertexBitangent,0.)).xyz );
	TBN = mat3(T,B,N);

	gl_Position = Matrix_MVP * vec4(aVertexPosition, 1.);
}