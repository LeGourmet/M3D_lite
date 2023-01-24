#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

uniform mat4 uMVPMatrix;
uniform vec3 uCamPos;

out vec3 normal;
out vec3 fragPos;
out vec3 camPos;

void main()
{
	vec4 pos = uMVPMatrix * vec4( aVertexPosition, 1.f );

	normal = vec4(aVertexNormal,1.f).xyz;
	camPos = uCamPos;
	fragPos = pos.xyz;

	gl_Position = pos;
}