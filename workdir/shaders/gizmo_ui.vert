#version 460
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 uvs;
layout(location = 2) in int aid;
layout(std140, binding = 0)uniform TransformsUBO{
    mat4 view_matrix;   // 0 ~ 64
} ubo;

layout (location = 0) flat out int id;

void main(){
    id = aid;
    gl_Position = ubo.view_matrix * vec4(aPos, 1.0) + vec4(.9, .8, 0., 0.);
}