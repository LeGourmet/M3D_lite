#version 460

out vec2 uv;

void main() {
	uv = vec2( (gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(uv*2.-1., 0., 1.);
}