#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uTexture;

uniform float uGamma; 

in vec2 uv;

void main(){
	vec3 col = texture2D(uTexture,uv).xyz;														// HDR 
	//col = col / (col + vec3(1.));																// Reinhard tone mapping
	//col = vec3(1.) - exp(-col * 5.);															// exposure tone mapping	(exposure = 5 here)
	col = clamp((col * (2.51 * col + 0.03)) / (col * (2.43 * col + 0.59) + 0.14), 0., 1.);		// ACES filmic tone mapping
	col = pow(col,vec3(1./uGamma));																// gama correction
	
	fragColor = vec4(col,1.);
}