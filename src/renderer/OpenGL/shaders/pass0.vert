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
	normal = vec4(aVertexNormal,1.f).xyz; // need to convert to world pos ?
	camPos = uCamPos;
	fragPos = uMVPMatrix * vec4( aVertexPosition, 1.f );

	gl_Position = fragPos;
}