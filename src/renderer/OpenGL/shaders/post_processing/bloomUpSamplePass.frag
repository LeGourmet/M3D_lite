#version 450

layout (location = 0) out vec3 fragColor;

layout( binding = 0 ) uniform sampler2D downSrcTexture;
layout( binding = 1 ) uniform sampler2D upSrcTexture;
uniform vec2 uInvSrcRes;

in vec2 uv;

void main()
{
    vec3 a = texture(downSrcTexture, uv+vec2(-1., 1.)*uInvSrcRes).xyz;
    vec3 b = texture(downSrcTexture, uv+vec2( 0., 1.)*uInvSrcRes).xyz;
    vec3 c = texture(downSrcTexture, uv+vec2( 1., 1.)*uInvSrcRes).xyz;

    vec3 d = texture(downSrcTexture, uv+vec2(-1., 0.)*uInvSrcRes).xyz;
    vec3 e = texture(downSrcTexture, uv+vec2( 0., 0.)*uInvSrcRes).xyz;
    vec3 f = texture(downSrcTexture, uv+vec2( 1., 0.)*uInvSrcRes).xyz;

    vec3 g = texture(downSrcTexture, uv+vec2(-1.,-1.)*uInvSrcRes).xyz;
    vec3 h = texture(downSrcTexture, uv+vec2( 0.,-1.)*uInvSrcRes).xyz;
    vec3 i = texture(downSrcTexture, uv+vec2( 1.,-1.)*uInvSrcRes).xyz;

    fragColor = mix(e*0.25 + (b+d+f+h)*0.125 + (a+c+g+i)*0.0625,texture(upSrcTexture, uv).xyz,0.5);
}