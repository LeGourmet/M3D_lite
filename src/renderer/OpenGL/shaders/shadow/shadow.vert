#version 460

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

layout(std430, binding = 0) buffer aTransformations { mat4 data_SSBO[]; };

uniform mat4 uLightMatrix_VP;

out vec2 uv;

void main()
{
	uv = aVertexTexCoords;
	gl_Position = uLightMatrix_VP * data_SSBO[gl_InstanceID*3] * vec4(aVertexPosition, 1.);
}