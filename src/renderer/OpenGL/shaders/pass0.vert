#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

uniform mat4 uMVPMatrix;
uniform vec3 uCamPos;

out vec2 uv;
out vec3 normal;
out vec3 fragPos;
out vec3 camPos;

void main()
{
	uv = aVertexTexCoords;
	normal = aVertexNormal; // need to convert to world pos ?
	camPos = uCamPos;

	vec4 pos = uMVPMatrix*vec4(aVertexPosition,1.);

	fragPos = pos.xyz/pos.w;
	gl_Position = pos;
}