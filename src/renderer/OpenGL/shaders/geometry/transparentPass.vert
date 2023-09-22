#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

layout(std430, binding = 0) buffer aTransformations { mat4 data_SSBO[]; };

out vec2 uv;

void main()
{
	mat4 Matrix_MVP = data_SSBO[gl_InstanceID*3+1];

	uv = aVertexTexCoords;

	gl_Position = Matrix_MVP * vec4(aVertexPosition, 1.);
}