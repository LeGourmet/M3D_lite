#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 uShadowTransform[6];

in vec2 uv[];

out vec4 fragPos;
out vec2 fragUVs;

void main()
{

    for(int i=0; i<6 ;i++){
        gl_Layer = i;
        for(int j=0; j<3 ;j++) {
            fragUVs = uv[j];
            fragPos = gl_in[j].gl_Position;
            gl_Position = uShadowTransform[i] * fragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  