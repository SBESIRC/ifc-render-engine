#version 460
layout(location = 0) in vec3 pos;

flat out vec3 color;
uniform mat4 mvp;

// layout(std140, binding = 0)uniform TransformMUBO{
// 	mat4 mvp;						// 0 ~ 64
// 	mat4 proj_view_model;			// 64 ~ 128
// 	mat3 transpose_inv_model;		// 128 ~ 176
// } ubo;

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