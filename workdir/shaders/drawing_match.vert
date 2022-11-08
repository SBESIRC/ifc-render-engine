#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout(std140, binding = 2)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
} ubo;

out vec2 TexCoords;

void main(){
	gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
	TexCoords = aTexCoords;
}