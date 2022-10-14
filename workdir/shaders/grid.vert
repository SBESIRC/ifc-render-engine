#version 430
layout(location = 0) in vec3 aPos;

layout(location = 1) in vec3 aColor;
layout(location = 2) in float alineType;

out float lineType;

flat out vec3 startPos;
out vec3 vertPos;

out vec3 lineColor;

layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
} ubo;

void main() {
    lineColor = aColor;
    lineType = alineType;
	vec4 pos    = ubo.proj_view_model * vec4(aPos, 1.0);
    gl_Position = pos;
    vertPos     = pos.xyz / pos.w;
    startPos    = vertPos;
}