#version 450

layout( location = 0 ) in vec2 aVertexPosition;

out vec2 uv;

void main(){
    float x = float(((uint(gl_VertexID) + 2u) / 3u)%2u); 
    float y = float(((uint(gl_VertexID) + 1u) / 3u)%2u); 
    uv = vec2(x, y);

    gl_Position = vec4(aVertexPosition,0., 1.);
}