#version 450

layout (location = 0) out vec3 fragColor;

layout( binding = 0 ) uniform sampler2D uSrcTexture;

uniform vec2 uInvSrcRes;

in vec2 uv;

void main()
{
    vec3 a = texture(uSrcTexture, uv+vec2(-2., 2.)*uInvSrcRes).xyz;
    vec3 b = texture(uSrcTexture, uv+vec2( 0., 2.)*uInvSrcRes).xyz;
    vec3 c = texture(uSrcTexture, uv+vec2( 2., 2.)*uInvSrcRes).xyz;
    vec3 d = texture(uSrcTexture, uv+vec2(-2., 0.)*uInvSrcRes).xyz;
    vec3 e = texture(uSrcTexture, uv+vec2( 0., 0.)*uInvSrcRes).xyz;
    vec3 f = texture(uSrcTexture, uv+vec2( 2., 0.)*uInvSrcRes).xyz;
    vec3 g = texture(uSrcTexture, uv+vec2(-2.,-2.)*uInvSrcRes).xyz;
    vec3 h = texture(uSrcTexture, uv+vec2( 0.,-2.)*uInvSrcRes).xyz;
    vec3 i = texture(uSrcTexture, uv+vec2( 2.,-2.)*uInvSrcRes).xyz;
    vec3 j = texture(uSrcTexture, uv+vec2(-1., 1.)*uInvSrcRes).xyz;
    vec3 k = texture(uSrcTexture, uv+vec2( 1., 1.)*uInvSrcRes).xyz;
    vec3 l = texture(uSrcTexture, uv+vec2(-1.,-1.)*uInvSrcRes).xyz;
    vec3 m = texture(uSrcTexture, uv+vec2( 1.,-1.)*uInvSrcRes).xyz;

    fragColor = max(vec3(0.000), (a+c+g+i)*0.03125 + (b+d+f+h)*0.0625 + (j+k+l+m+e)*0.125);
}