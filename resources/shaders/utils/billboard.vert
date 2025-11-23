#version 460

layout( location = 0 ) in vec3 aVertexPosition;

out vec2 uv;

void main(){
    uv = (aVertexPosition.xy+1.)*0.5;
    gl_Position = vec4(aVertexPosition,1.);
}