#version 460

layout (location = 0) out vec3 fragColor;

layout( binding = 0 ) uniform sampler2D uDownSrcTexture;
layout( binding = 1 ) uniform sampler2D uUpSrcTexture;

uniform vec2 uInvSrcRes;

in vec2 uv;

void main()
{
    vec3 a = texture(uDownSrcTexture, uv+vec2(-1., 1.)*uInvSrcRes).xyz;
    vec3 b = texture(uDownSrcTexture, uv+vec2( 0., 1.)*uInvSrcRes).xyz;
    vec3 c = texture(uDownSrcTexture, uv+vec2( 1., 1.)*uInvSrcRes).xyz;

    vec3 d = texture(uDownSrcTexture, uv+vec2(-1., 0.)*uInvSrcRes).xyz;
    vec3 e = texture(uDownSrcTexture, uv+vec2( 0., 0.)*uInvSrcRes).xyz;
    vec3 f = texture(uDownSrcTexture, uv+vec2( 1., 0.)*uInvSrcRes).xyz;

    vec3 g = texture(uDownSrcTexture, uv+vec2(-1.,-1.)*uInvSrcRes).xyz;
    vec3 h = texture(uDownSrcTexture, uv+vec2( 0.,-1.)*uInvSrcRes).xyz; 
    vec3 i = texture(uDownSrcTexture, uv+vec2( 1.,-1.)*uInvSrcRes).xyz;

    fragColor = (e*0.25 + (b+d+f+h)*0.125 + (a+c+g+i)*0.0625 + texture(uUpSrcTexture, uv).xyz)*0.5; 
}