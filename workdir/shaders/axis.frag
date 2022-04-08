#version 450 core

flat in vec3 color;
// out vec4 FragColor;
void main(){
    // gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    gl_FragColor = vec4(color, 1.0);
}