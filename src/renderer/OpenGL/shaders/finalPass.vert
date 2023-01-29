#version 450

out vec2 uv;

void main(){
	float x = float(((uint(gl_VertexID) + 2u) / 3u)%2u); 
    float y = float(((uint(gl_VertexID) + 1u) / 3u)%2u); 

    uv = vec2(x, y);
    gl_Position = vec4(x*2.-1., y*2.-1., 0., 1.);

    /*vec2[6] pos = vec2[6](vec2(-1.,-1.),vec2(1.,-1.),vec2(-1.,1.),vec2(1.,-1.),vec2(-1.,1.),vec2(1.,1.));
    vec2[6] uvs = vec2[6](vec2(0.,0.),vec2(1.,0.),vec2(0.,1.),vec2(1.,0.),vec2(0.,1.),vec2(1.,1.));

    uv = uvs[gl_VertexID];
    gl_Position = vec4(pos[gl_VertexID],0.,1.);*/
}