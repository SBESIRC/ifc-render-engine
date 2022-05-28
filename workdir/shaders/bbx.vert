#version 460
layout(location = 0) in vec3 aPos;
layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
} ubo;
void main()
{
	// gl_Position = projection * modelview * vec4(aPos, 1.0);
	gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
}