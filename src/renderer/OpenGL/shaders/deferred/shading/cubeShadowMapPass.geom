#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 uShadowTransform[6];

out vec4 fragPos;

void main()
{
    for(int i=0; i<6 ;i++){
        gl_Layer = i;
        for(int j=0; j<3 ;j++) {
            fragPos = gl_in[j].gl_Position;
            gl_Position = uShadowTransform[i] * fragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  