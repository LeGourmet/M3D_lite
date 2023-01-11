#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 2 ) in vec3 aVertexNormal;
layout( location = 3 ) in vec2 aVertexTexCoords;
layout( location = 4 ) in vec3 aVertexTangent;
layout( location = 5 ) in vec3 aVertexBitangent;

uniform mat4 uMVPMatrix;
uniform mat4 uMMatrix;
uniform vec3 uCamPos;

out vec2 uv;
out vec3 normal;
out vec3 fragPos;
out vec3 camPos;
out mat3 TBN;

void main()
{
	uv = aVertexTexCoords;
	normal = vec4(aVertexNormal,1.f).xyz;
	camPos = uCamPos;
	fragPos = (uMVPMatrix * vec4( aVertexPosition, 1.f )).xyz;

	vec3 T	 = normalize( (uMMatrix * vec4(aVertexTangent,0.f)).xyz );
	vec3 B	 = normalize( (uMMatrix * vec4(aVertexBitangent,0.f)).xyz );
	vec3 N	 = normalize( (uMMatrix * vec4(aVertexNormal,0.f)).xyz );
	TBN = mat3(T,B,N);

	gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
}
