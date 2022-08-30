#version 430
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 uvs;
layout(std140, binding = 0)uniform TransformMVPUBO{
    mat4 view_matrix;   // 0 ~ 64
} ubo;

out vec2 TexCoord;

void main(){
    TexCoord = uvs;
    gl_Position = ubo.view_matrix * vec4(aPos, 1.0) + vec4(.9, -.8, 0., 0.);
}