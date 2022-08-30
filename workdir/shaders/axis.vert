#version 430
layout(location = 0) in vec3 aPos;

layout(location = 0) flat out vec3 color;

layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
} ubo;

void main(){
    int index = gl_VertexID >> 1;
    switch(index){
        case 0: color = vec3(1.0, 0.0, 0.0); break;
        case 1: color = vec3(0.0, 0.0, 1.0); break;
        case 2: color = vec3(0.0, 1.0, 0.0); break;
        default: color = vec3(1.0, 1.0, 1.0); break;
    }
    gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
}