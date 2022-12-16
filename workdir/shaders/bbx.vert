#version 460
layout(location = 0) in vec3 aPos;
layout(std140, binding = 0)uniform TransformsUBO{
	mat4 proj_view_model;			// 0 ~ 64
	mat4 model;						// 64 ~ 128
	vec4 uUserClipPlane;			// 128 ~ 144
	vec4 uUserClipBox[6];			// 144 ~ 240
	vec4 drawing_plane;				// 240 ~ 256
	int showTileView;				// 256 ~ 260
} ubo;


uniform mat4 storeyOffset_mat;

void main()
{
	gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
}