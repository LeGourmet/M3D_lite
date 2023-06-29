#version 450

layout (location = 0) out vec3 fragColor;

layout( binding = 0 ) uniform sampler2D srcTexture; // edge clamp + bilinear mignification + hdr
uniform float filterRadius;

in vec2 uv;

void main()
{
    vec3 a = texture(srcTexture, uv+vec2(-1., 1.)*filterRadius).xyz;
    vec3 b = texture(srcTexture, uv+vec2( 0., 1.)*filterRadius).xyz;
    vec3 c = texture(srcTexture, uv+vec2( 1., 1.)*filterRadius).xyz;

    vec3 d = texture(srcTexture, uv+vec2(-1., 0.)*filterRadius).xyz;
    vec3 e = texture(srcTexture, uv+vec2( 0., 0.)*filterRadius).xyz;
    vec3 f = texture(srcTexture, uv+vec2( 1., 0.)*filterRadius).xyz;

    vec3 g = texture(srcTexture, uv+vec2(-1.,-1.)*filterRadius).xyz;
    vec3 h = texture(srcTexture, uv+vec2( 0.,-1.)*filterRadius).xyz;
    vec3 i = texture(srcTexture, uv+vec2( 1.,-1.)*filterRadius).xyz;

    fragColor = (e*4. + (b+d+f+h)*2. + (a+c+g+i))/16.;
}