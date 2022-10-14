#version 430

in vec3 lineColor;

layout(location = 0) out vec4 FragColor;

void main(){
    FragColor = vec4(lineColor, 1.0);
}