#version 430
layout(location = 0) in vec3 aPos;
layout(location = 1) in int this_id;
layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
} ubo;

layout (location = 0)flat out int thisid;

void main()
{
	thisid = this_id;
	// gl_Position = projection * modelview * vec4(aPos, 1.0);
	gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
}