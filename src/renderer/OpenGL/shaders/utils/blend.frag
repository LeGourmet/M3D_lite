#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uTexture0;
layout( binding = 1 ) uniform sampler2D uTexture1;

in vec2 uv;

void main()
{
	vec4 tex0 = texture(uTexture0,uv);
	vec4 tex1 = texture(uTexture1,uv);

	if(tex1.a<1.f)	{ fragColor = vec4(mix(tex0.xyz,tex1.xyz,tex1.a),1.); }
	else			{ fragColor = tex0; }	
}