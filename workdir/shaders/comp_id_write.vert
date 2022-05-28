#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in int aCompId;

layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
} ubo;

layout (location = 0) flat out int vCompId;

void main()
{   
    vCompId = aCompId;
    // gl_Position = mvp * vec4(pos, 1.0);
    gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
}
