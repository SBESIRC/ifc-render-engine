#version 460
layout (location = 0) in vec3 pos;
layout (location = 3) in int aComp; //todo

layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
} ubo;

layout (location = 0) flat out int vCompId;

void main()
{   
    vCompId = aComp;
    // gl_Position = mvp * vec4(pos, 1.0);
    gl_Position = ubo.proj_view_model * vec4(pos, 1.0);
}
