#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

uniform mat4 uMatrix_MVP;
uniform mat4 uMatrix_M;
uniform mat4 uMatrix_Normal;

out vec2 uv;
out vec3 fragNormal;
out vec3 fragPosition;
out mat3 TBN;

void main()
{
	uv = aVertexTexCoords;
	fragNormal = (uMatrix_Normal * vec4(aVertexNormal,1.)).xyz;
	fragPosition = (uMatrix_M * vec4(aVertexPosition,1.)).xyz;

	/* // Gram-Schmidt process
	vec3 T	 = normalize( (uMatrix_M * vec4(aVertexTangent,0.)).xyz );
	vec3 N	 = normalize( (uMatrix_M * vec4(aVertexNormal,0.)).xyz );
	T = normalize(T - dot(T, N) * N);
	vec3 B	 = cross(N, T); // non normalize ?
	TBN = mat3(T,B,N);
	*/

	vec3 T	 = normalize( (uMatrix_M * vec4(aVertexTangent,0.)).xyz );
	vec3 B	 = normalize( (uMatrix_M * vec4(aVertexBitangent,0.)).xyz );
	vec3 N	 = normalize( (uMatrix_M * vec4(aVertexNormal,0.)).xyz );
	TBN = mat3(T,B,N);

	gl_Position = uMatrix_MVP * vec4(aVertexPosition, 1.);
}