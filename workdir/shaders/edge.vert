#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in int aCompId;

layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
	mat4 model;					//64 ~ 128
    vec4 clip_plane;            //128 ~ 144
} ubo;

layout(location = 0) out vec3 vGoColor;
layout(location = 1) flat out float vDistance;

void main()
{
	vGoColor = aColor * 0.5;
	vec4 p = vec4(aPos, 1.0);
	vec4 eyePos = ubo.model * p;
	vDistance = dot(eyePos.xyz, ubo.clip_plane.xyz) - ubo.clip_plane.w;
	gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
}