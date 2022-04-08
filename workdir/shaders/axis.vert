#version 450 core
layout(location = 0) in vec3 pos;

flat out vec3 color;
uniform mat4 mvp;

void main(){
    int index = gl_VertexID >> 1;
    switch(index){
        case 0: color = vec3(1.0, 0.0, 0.0);break;
        case 1: color = vec3(0.0, 0.0, 1.0);break;
        case 2: color = vec3(0.0, 1.0, 0.0);break;
        default: color = vec3(1.0, 1.0, 1.0);break;
    }
    gl_Position = mvp * vec4(pos, 1.0);
}