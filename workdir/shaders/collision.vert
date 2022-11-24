#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in int aCompId;
layout(location = 4) in int alou;

layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
	mat4 model;					// 64 ~ 128
    vec4 clip_plane;            // 128 ~ 144
	vec4 uUserClipBox[6];		// 144 ~ 240
} ubo;

layout(std140, binding = 3)uniform StoreyOffsetTransformUBO{
	mat4 storeyOffset_mat[100];		// 0 ~ 6400
} sotubo;
layout(location = 0) out float vDistance;
layout(location = 1) out float vDistanceM[6];

void main()
{
	vec4 p = vec4(aPos, 1.0);
	vec4 eyePos = ubo.model * sotubo.storeyOffset_mat[alou] * p;
	vDistance = dot(eyePos.xyz, ubo.clip_plane.xyz) - ubo.clip_plane.w;
	for(int i=0;i<6;i++){
		vDistanceM[i]=dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w;
	}
	gl_Position = ubo.proj_view_model * sotubo.storeyOffset_mat[alou] * vec4(aPos, 1.0);
}