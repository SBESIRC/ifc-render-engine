#version 430
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 uvs;
layout(std140, binding = 0)uniform TransformMVPUBO{
    mat4 view_matrix;   // 0 ~ 64
} ubo;

uniform vec2 offset;

out vec2 TexCoord;

void main(){
    TexCoord = uvs;
    vec4 temp = ubo.view_matrix * vec4(aPos, 1.0);
    gl_Position = temp + vec4(offset, 0.0, 1.0);
}