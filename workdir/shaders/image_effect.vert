#version 460
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texcoord;

out vec2 f_texcoord[5];

float _SampleDistance = 1.0;
uniform vec2 screenTexTexelSize;
uniform float scale;

void main(){
    f_texcoord[0] = texcoord;
    f_texcoord[1] = texcoord + screenTexTexelSize.xy * vec2(1, 1) * _SampleDistance;
    f_texcoord[2] = texcoord + screenTexTexelSize.xy * vec2(-1, -1) * _SampleDistance;
	f_texcoord[3] = texcoord + screenTexTexelSize.xy * vec2(-1, 1) * _SampleDistance;
	f_texcoord[4] = texcoord + screenTexTexelSize.xy * vec2(1, -1) * _SampleDistance;
    vec2 nowpos = pos * scale + vec2(1. - scale);
    gl_Position = vec4(nowpos, 0.0, 1.0);
}