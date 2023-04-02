#version 450

layout( location = 0 ) in vec2 aVertexPosition;

out vec2 uv;

void main(){
    uv = (aVertexPosition+1.)*0.5;
    gl_Position = vec4(aVertexPosition,0., 1.);
}