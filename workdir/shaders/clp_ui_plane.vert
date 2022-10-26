#version 460
layout(location = 0) in vec3 aPos;
layout(location = 1) in int aid;
layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
} ubo;

layout (location = 0) flat out int id;

void main()
{
	id = aid;
	gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
}