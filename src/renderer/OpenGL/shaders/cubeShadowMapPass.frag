#version 450

uniform vec3 uLightPos;
uniform float uZfar;

in vec4 fragPos;

void main()
{
    gl_FragDepth = length(fragPos.xyz - uLightPos)/uZfar;
}  