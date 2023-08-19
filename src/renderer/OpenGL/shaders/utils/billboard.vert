#version 450

layout(std430, binding = 0) buffer aVertex { vec4 data_SSBO[]; };

out vec2 uv;

void main(){
    vec4 pos = data_SSBO[gl_VertexID%3 + gl_VertexID/3];
    uv = (pos.xy+1.)*0.5;
    gl_Position = vec4(pos);
}