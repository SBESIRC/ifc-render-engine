#version 450 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texcoord;
out vec2 f_texcoord[5];

float _SampleDistance;

void main(){
    f_texcoord[0] = texcoord;
    
    
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}