#version 460
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D Drawing;

void main(){
	vec4 color = texture(Drawing, TexCoords);
	if(color.a < 0.1)
		discard;
	FragColor = vec4(color.rgb, 0.6);
}