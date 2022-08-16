#version 430
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in int aCompId;

layout(std140, binding = 0)uniform TransformMVPUBO{
	mat4 proj_view_model;       // 0 ~ 64
	mat4 model;					// 64 ~ 128
    vec4 clip_plane;            // 128 ~ 144
	vec4 uUserClipBox[6];		// 144 ~ 240
} ubo;

layout(location = 0) out vec3 vGoColor;
layout(location = 1) out float vDistance;
layout(location = 2) out float vDistanceM[6];

void main()
{
	vGoColor = aColor * 0.5;
	vec4 p = vec4(aPos, 1.0);
	vec4 eyePos = ubo.model * p;
	vDistance = dot(eyePos.xyz, ubo.clip_plane.xyz) - ubo.clip_plane.w;
	for(int i=0;i<6;i++){
		vDistanceM[i]=dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w;
		//if(dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w<0.0){
		//	vDistanceM= -1.0;
		//	break;
		//}
	}
	gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);
}