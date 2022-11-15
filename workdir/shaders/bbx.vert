#version 460
layout(location = 0) in vec3 aPos;
layout(location = 1) in int alou;
layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
	mat4 model;					//64 ~ 128
    vec4 clip_plane;            //128 ~ 144
	vec4 uUserClipBox[6];		// 144 ~ 240
	int showTileView;			// 240 ~ 244
} ubo;

layout(std140, binding = 3)uniform StoreyOffsetTransformUBO{
	mat4 storeyOffset_mat[100];		// 0 ~ 6400
	int floorIndex[100];			// 6400 ~ 6800
} sotubo;

void main()
{
	// gl_Position = projection * modelview * vec4(aPos, 1.0);
	if(ubo.showTileView > 0)
		gl_Position = ubo.proj_view_model * sotubo.storeyOffset_mat[alou] * vec4(aPos, 1.0);
	else
		gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
}