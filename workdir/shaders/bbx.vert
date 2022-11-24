#version 460
layout(location = 0) in vec3 aPos;
layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
	mat4 model;					//64 ~ 128
    vec4 clip_plane;            //128 ~ 144
	vec4 uUserClipBox[6];		// 144 ~ 240
	int showTileView;			// 240 ~ 244
} ubo;

uniform mat4 storeyOffset_mat;

void main()
{
	// gl_Position = projection * modelview * vec4(aPos, 1.0);
	// if(ubo.showTileView > 0)
	// 	gl_Position = ubo.proj_view_model * storeyOffset_mat * vec4(aPos, 1.0);
	// else
	 	gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
}