﻿#pragma once
#ifndef IFCRE_SHADER_CONSTS_H_
#define IFCRE_SHADER_CONSTS_H_

namespace ifcre {
	namespace sc {
		static const char* f_axis = "#version 430\r\n"
			"layout(location = 0) flat in vec3 color;\r\n"
			"layout(location = 0) out vec4 FragColor;\r\n"
			"void main(){\r\n"
			"FragColor = vec4(color, 1.0);\r\n"
			"}\r\n";
		static const char* v_axis = "#version 430\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 0) flat out vec3 color;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"} ubo;\r\n"
			"void main(){\r\n"
			"int index = (gl_VertexID >> 1) % 3;\r\n"
			"switch(index){\r\n"
			"case 0: color = vec3(1.0, 0.0, 0.0); break;\r\n"
			"case 1: color = vec3(0.0, 0.0, 1.0); break;\r\n"
			"case 2: color = vec3(0.0, 1.0, 0.0); break;\r\n"
			"default: color = vec3(1.0, 1.0, 0.0); break;\r\n"
			"}\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";

		static const char* v_axis_vk = "#version 460\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 0) flat out vec3 color;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"} ubo;\r\n"
			"void main(){\r\n"
			"int index = gl_VertexIndex >> 1;\r\n"
			"switch(index){\r\n"
			"case 0: color = vec3(1.0, 0.0, 0.0); break;\r\n"
			"case 1: color = vec3(0.0, 0.0, 1.0); break;\r\n"
			"case 2: color = vec3(0.0, 1.0, 0.0); break;\r\n"
			"default: color = vec3(1.0, 1.0, 1.0); break;\r\n"
			"}\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_bbx = "#version 430\r\n"
			"layout(location = 0) out vec4 FragColor;\r\n"
			"void main()\r\n"
			"{\r\n"
			"FragColor = vec4(1.0, 0.6, 0.0, 1.0);\r\n"
			"}\r\n";

		static const char* v_bbx = "#version 430\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"mat4 model;					//64 ~ 128\r\n"
			"vec4 clip_plane;            //128 ~ 144\r\n"
			"} ubo;\r\n"
			"void main()\r\n"
			"{\r\n"
			"// gl_Position = projection * modelview * vec4(aPos, 1.0);\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_chosen_test = "#version 430\r\n"
			"layout(location = 0) in vec3 vGoColor;\r\n"
			"layout(location = 1) in vec3 vNormal;\r\n"
			"layout(location = 2) in vec3 vFragPos;\r\n"
			"layout(location = 3) flat in int vComp;\r\n"
			"layout(location = 4) in float vDistance;\r\n"
			"layout(location = 5) in float vDistanceM[6];\r\n"
			"layout(std140, binding = 1)uniform IFCRenderUBO{\r\n"
			"float alpha;			// 0 ~ 4\r\n"
			"int c_comp;				// 4 ~ 8\r\n"
			"int h_comp;				// 8 ~ 12\r\n"
			"vec3 cameraDirection;	// 16 ~ 32\r\n"
			"} ubo;\r\n"
			"layout(location = 0) out vec4 FragColor;\r\n"
			"void main()\r\n"
			"{\r\n"
			"for(int i=0;i<6;i++){\r\n"
			"if(vDistanceM[i]<0.0)\r\n"
			"discard;\r\n"
			"}\r\n"
			"if(vDistance<0.0)\r\n"
			"discard;\r\n"
			"vec3 color = vGoColor / 2.0 + vec3(0.0, 0.5, 0.0);\r\n"
			"if (ubo.h_comp == vComp) {\r\n"
			"color = vGoColor / 4.0 * 3.0 + vec3(0.0, 0.25, 0.0);\r\n"
			"}\r\n"
			"vec3 norm = normalize(vNormal);\r\n"
			"vec3 diffuse = max(dot(norm, ubo.cameraDirection), 0.0) * color * 0.3;\r\n"
			"color = color * 0.7 + diffuse;\r\n"
			"FragColor = vec4(color, ubo.alpha);\r\n"
			"}\r\n";

		static const char* v_chosen_test = "#version 430\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 1) in vec3 aNormal;\r\n"
			"layout(location = 2) in vec3 aColor;\r\n"
			"layout(location = 3) in int aComp;\r\n"
			"layout(std140, binding = 0)uniform TransformsUBO{\r\n"
			"mat4 view_model;				// 0 ~ 64\r\n"
			"mat4 proj_view_model;			// 64 ~ 128\r\n"
			"mat3 transpose_inv_model;		// 128 ~ 176\r\n"
			"vec4 uUserClipPlane;			// 176 ~ 192\r\n"
			"mat4 model;						// 192 ~ 256\r\n"
			"vec4 uUserClipBox[6];			// 256 ~ 352\r\n"
			"} ubo;\r\n"
			"layout(location = 0) out vec3 vGoColor;\r\n"
			"layout(location = 1) out vec3 vNormal;\r\n"
			"layout(location = 2) out vec3 vFragPos;\r\n"
			"layout(location = 3) flat out int vComp;\r\n"
			"layout(location = 4) out float vDistance;\r\n"
			"layout(location = 5) out float vDistanceM[6];\r\n"
			"void main()\r\n"
			"{\r\n"
			"vGoColor = aColor;\r\n"
			"vec4 p = vec4(aPos, 1.0);\r\n"
			"vec4 eyePos = ubo.model * p;\r\n"
			"vFragPos = eyePos.xyz;\r\n"
			"vDistance = dot(vFragPos, ubo.uUserClipPlane.xyz) - ubo.uUserClipPlane.w;\r\n"
			"for(int i=0;i<6;i++){\r\n"
			"vDistanceM[i]=dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w;\r\n"
			"//if(dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w<0.0){\r\n"
			"//	vDistanceM= -1.0;\r\n"
			"//	break;\r\n"
			"//}\r\n"
			"}\r\n"
			"vComp = aComp;\r\n"
			"//vNormal = ubo.transpose_inv_model * aNormal;\r\n"
			"vNormal = aNormal;\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_clp_plane = "#version 430\r\n"
			"layout(location = 0) flat in int thisid;\r\n"
			"layout(location = 0) out vec4 FragColor;\r\n"
			"uniform int ui_id;\r\n"
			"void main(){\r\n"
			"FragColor = vec4(0. ,1. ,1. , .2);\r\n"
			"if(ui_id == thisid){\r\n"
			"FragColor = vec4(0.5 ,.5 ,1. , 1.);\r\n"
			"}\r\n"
			"}\r\n";

		static const char* v_clp_plane = "#version 430\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 1) in int this_id;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"} ubo;\r\n"
			"layout (location = 0)flat out int thisid;\r\n"
			"void main()\r\n"
			"{\r\n"
			"thisid = this_id;\r\n"
			"// gl_Position = projection * modelview * vec4(aPos, 1.0);\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_clp_ui_plane = "#version 430\r\n"
			"layout(location = 0) out int FragId;\r\n"
			"layout(location = 0) flat in int id;\r\n"
			"void main(){\r\n"
			"FragId = id;\r\n"
			"}\r\n";

		static const char* v_clp_ui_plane = "#version 430\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 1) in int aid;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"} ubo;\r\n"
			"layout (location = 0) flat out int id;\r\n"
			"void main()\r\n"
			"{\r\n"
			"id = aid;\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_collision = "#version 430\r\n"
			"#define PI 3.14159265358979323846\r\n"
			"layout(location = 0)in float vDistance;\r\n"
			"layout(location = 1)in float vDistanceM[6];\r\n"
			"uniform vec2 u_resolution;\r\n"
			"uniform float alpha;\r\n"
			"layout(location = 0)out vec4 FragColor;\r\n"
			"vec2 rotate2D(vec2 _st,float _angle){\r\n"
			"_st-=0.5;\r\n"
			"_st = mat2(cos(_angle),-sin(_angle),\r\n"
			"sin(_angle),cos(_angle))*_st;\r\n"
			"_st+=0.5;\r\n"
			"return _st;\r\n"
			"}\r\n"
			"vec2 tile(vec2 _st, float _zoom){\r\n"
			"_st*=_zoom;\r\n"
			"return fract(_st);\r\n"
			"}\r\n"
			"void main()\r\n"
			"{\r\n"
			"for(int i=0;i<6;i++){\r\n"
			"if(vDistanceM[i]<0.0)\r\n"
			"discard;\r\n"
			"}\r\n"
			"if(vDistance<0.0)\r\n"
			"discard;\r\n"
			"vec2 st = gl_FragCoord.xy/u_resolution;\r\n"
			"vec3 outColor=vec3(1.0,0.3729,0.4061);\r\n"
			"vec3 one=vec3(1.,1.,1.);\r\n"
			"st=tile(st,7.0);\r\n"
			"st=rotate2D(st,PI*0.25);\r\n"
			"float fin = mod(st.x,.1);\r\n"
			"fin=smoothstep(.04,.06,fin);\r\n"
			"outColor=mix(outColor,one ,fin*.5);\r\n"
			"FragColor = vec4(outColor, alpha);\r\n"
			"}\r\n";

		static const char* v_collision = "#version 430\r\n"
			"layout (location = 0) in vec3 aPos;\r\n"
			"layout (location = 1) in vec3 aNormal;\r\n"
			"layout (location = 2) in vec3 aColor;\r\n"
			"layout (location = 3) in int aCompId;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"mat4 model;					// 64 ~ 128\r\n"
			"vec4 clip_plane;            // 128 ~ 144\r\n"
			"vec4 uUserClipBox[6];		// 144 ~ 240\r\n"
			"} ubo;\r\n"
			"layout(location = 0) out float vDistance;\r\n"
			"layout(location = 1) out float vDistanceM[6];\r\n"
			"void main()\r\n"
			"{\r\n"
			"vec4 p = vec4(aPos, 1.0);\r\n"
			"vec4 eyePos = ubo.model * p;\r\n"
			"vDistance = dot(eyePos.xyz, ubo.clip_plane.xyz) - ubo.clip_plane.w;\r\n"
			"for(int i=0;i<6;i++){\r\n"
			"vDistanceM[i]=dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w;\r\n"
			"}\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_comp_id_write = "#version 430\r\n"
			"layout(location = 0) out int FragId;\r\n"
			"layout(location = 0) flat in int vCompId;\r\n"
			"layout(location = 1) in float vDistance;\r\n"
			"layout(location = 2) in float vDistanceM[6];\r\n"
			"// vec3 int2rgb(int num){\r\n"
			"//     float b = (num % 256) / 256.;\r\n"
			"//     num = num >> 8;\r\n"
			"//     float g = (num % 256) / 256.;\r\n"
			"//     num = num >> 8;\r\n"
			"//     float r = (num % 256) / 256.;\r\n"
			"//     return vec3(r, g, b);\r\n"
			"// }\r\n"
			"void main(){\r\n"
			"for(int i=0;i<6;i++){\r\n"
			"if(vDistanceM[i]<0.0)\r\n"
			"discard;\r\n"
			"}\r\n"
			"if(vDistance<0.0)\r\n"
			"//FragId=-1;\r\n"
			"discard;\r\n"
			"// FragId=vec4(int2rgb(comp),1.);\r\n"
			"FragId = vCompId;\r\n"
			"}\r\n";

		static const char* v_comp_id_write = "#version 430\r\n"
			"layout (location = 0) in vec3 aPos;\r\n"
			"layout (location = 1) in vec3 aNormal;\r\n"
			"layout (location = 2) in vec3 aColor;\r\n"
			"layout (location = 3) in int aCompId;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"mat4 model;					//64 ~ 128\r\n"
			"vec4 clip_plane;            //128 ~ 144\r\n"
			"vec4 uUserClipBox[6];		// 144 ~ 240\r\n"
			"} ubo;\r\n"
			"layout (location = 0) flat out int vCompId;\r\n"
			"layout (location = 1) out float vDistance;\r\n"
			"layout (location = 2) out float vDistanceM[6];\r\n"
			"void main()\r\n"
			"{\r\n"
			"vCompId = aCompId;\r\n"
			"vec4 p = vec4(aPos, 1.0);\r\n"
			"vec4 eyePos = ubo.model * p;\r\n"
			"vDistance = dot(eyePos.xyz, ubo.clip_plane.xyz) - ubo.clip_plane.w;\r\n"
			"for(int i=0;i<6;i++){\r\n"
			"vDistanceM[i]=dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w;\r\n"
			"//if(dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w<0.0){\r\n"
			"//	vDistanceM= -1.0;\r\n"
			"//	break;\r\n"
			"//}\r\n"
			"}\r\n"
			"// gl_Position = mvp * vec4(pos, 1.0);\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_edge = "#version 430\r\n"
			"layout(location = 0)in vec3 vGoColor;\r\n"
			"layout(location = 1)in float vDistance;\r\n"
			"layout(location = 2)in float vDistanceM[6];\r\n"
			"layout(location = 0)out vec4 FragColor;\r\n"
			"void main()\r\n"
			"{\r\n"
			"for(int i=0;i<6;i++){\r\n"
			"if(vDistanceM[i]<0.0)\r\n"
			"discard;\r\n"
			"}\r\n"
			"if(vDistance<0.0)\r\n"
			"discard;\r\n"
			"FragColor = vec4(vGoColor,1.0);\r\n"
			"}\r\n";

		static const char* v_edge = "#version 430\r\n"
			"layout (location = 0) in vec3 aPos;\r\n"
			"layout (location = 1) in vec3 aNormal;\r\n"
			"layout (location = 2) in vec3 aColor;\r\n"
			"layout (location = 3) in int aCompId;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"mat4 model;					// 64 ~ 128\r\n"
			"vec4 clip_plane;            // 128 ~ 144\r\n"
			"vec4 uUserClipBox[6];		// 144 ~ 240\r\n"
			"} ubo;\r\n"
			"layout(location = 0) out vec3 vGoColor;\r\n"
			"layout(location = 1) out float vDistance;\r\n"
			"layout(location = 2) out float vDistanceM[6];\r\n"
			"void main()\r\n"
			"{\r\n"
			"vGoColor = aColor * 0.5;\r\n"
			"vec4 p = vec4(aPos, 1.0);\r\n"
			"vec4 eyePos = ubo.model * p;\r\n"
			"vDistance = dot(eyePos.xyz, ubo.clip_plane.xyz) - ubo.clip_plane.w;\r\n"
			"for(int i=0;i<6;i++){\r\n"
			"vDistanceM[i]=dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w;\r\n"
			"//if(dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w<0.0){\r\n"
			"//	vDistanceM= -1.0;\r\n"
			"//	break;\r\n"
			"//}\r\n"
			"}\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_gizmo = "#version 460\r\n"
			"layout(location = 0) out vec4 FragColor;\r\n"
			"in vec2 TexCoord;// 传入纹理坐标\r\n"
			"in flat int thisid;\r\n"
			"uniform sampler2D ourTexture;// 把一个纹理添加到片段着色器中\r\n"
			"void main(){\r\n"
			"vec4 tempColor = texture(ourTexture,TexCoord);// 采样纹理的颜色(纹理采样器,对应的纹理坐标)\r\n"
			"if(thisid > 0){\r\n"
			"tempColor = tempColor*0.75+ vec4(0., 0.25, 0.25, 0.);\r\n"
			"}\r\n"
			"FragColor = tempColor;\r\n"
			"}\r\n";

		static const char* v_gizmo = "#version 460\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 1) in vec2 uvs;\r\n"
			"layout(location = 2) in int aid;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 view_matrix;   // 0 ~ 64\r\n"
			"} ubo;\r\n"
			"uniform int hover_id;\r\n"
			"out vec2 TexCoord;// 传出纹理坐标\r\n"
			"out flat int thisid;\r\n"
			"void main(){\r\n"
			"TexCoord = uvs;\r\n"
			"gl_Position = ubo.view_matrix * vec4(aPos, 1.0) + vec4(.9, .8, 0., 0.);\r\n"
			"thisid = 0;\r\n"
			"if(hover_id == aid)\r\n"
			"thisid = 1;\r\n"
			"}\r\n";
		static const char* f_gizmo_ui = "#version 430\r\n"
			"layout(location = 0) out int FragId;\r\n"
			"layout(location = 0) flat in int id;\r\n"
			"void main(){\r\n"
			"FragId = id;\r\n"
			"}\r\n";

		static const char* v_gizmo_ui = "#version 430\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 1) in vec2 uvs;\r\n"
			"layout(location = 2) in int aid;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 view_matrix;   // 0 ~ 64\r\n"
			"} ubo;\r\n"
			"layout (location = 0) flat out int id;\r\n"
			"void main(){\r\n"
			"id = aid;\r\n"
			"gl_Position = ubo.view_matrix * vec4(aPos, 1.0) + vec4(.9, .8, 0., 0.);\r\n"
			"}\r\n";
		static const char* f_grid = "#version 430\r\n"
			"layout(location = 0) flat in vec3 color;\r\n"
			"layout(location = 0) out vec4 FragColor;\r\n"
			"void main(){\r\n"
			"FragColor = vec4(color, 1.0);\r\n"
			"}\r\n";

		static const char* v_grid = "#version 430\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 0) flat out vec3 color;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"} ubo;\r\n"
			"void main(){\r\n"
			"int index = (gl_VertexID >> 1) % 3;\r\n"
			"switch(index){\r\n"
			"case 0: color = vec3(1.0, 0.0, 0.0); break;\r\n"
			"case 1: color = vec3(0.0, 0.0, 1.0); break;\r\n"
			"case 2: color = vec3(0.0, 1.0, 0.0); break;\r\n"
			"default: color = vec3(1.0, 1.0, 0.0); break;\r\n"
			"}\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_image_effect = "#version 430\r\n"
			"out vec4 FragColor;\r\n"
			"in vec2 f_texcoord[5];\r\n"
			"uniform sampler2D screenTexture;\r\n"
			"uniform sampler2D depthNormalTexture;\r\n"
			"const vec2 _Sensitivity = vec2(1.0, 1.0);\r\n"
			"const vec4 _EdgeColor = vec4(0.0, 0.0, 0.0, 1.0);\r\n"
			"const vec4 _BackgroundColor = vec4(1.0, 1.0, 1.0, 1.0);\r\n"
			"const float _EdgeOnly = 0.5f;\r\n"
			"float DecodeFloatRG(in vec2 enc)\r\n"
			"{\r\n"
			"vec2 kDecodeDot = vec2(1.0, 1/255.0);\r\n"
			"return dot( enc, kDecodeDot );\r\n"
			"}\r\n"
			"float CheckSame(in vec4 col1,in vec4 col2){\r\n"
			"float d1 = DecodeFloatRG(col1.zw);\r\n"
			"vec2 normal1 = col1.xy;\r\n"
			"float d2 = DecodeFloatRG(col2.zw);\r\n"
			"vec2 normal2 = col2.xy;\r\n"
			"vec2 diffNormal = abs(normal2 - normal1) * _Sensitivity.x;\r\n"
			"float diffDepth = abs(d2 - d1) * _Sensitivity.y;\r\n"
			"int isSameNormal = (diffNormal.x + diffNormal.y) < 0.1?1:0;\r\n"
			"int isSameDepth = (diffDepth < (0.1 * d1)) ? 1:0;\r\n"
			"return (isSameDepth * isSameNormal != 0) ? 1.0 : 0.0;\r\n"
			"}\r\n"
			"void main(){\r\n"
			"vec4 sample1 = texture(depthNormalTexture, f_texcoord[1]);\r\n"
			"vec4 sample2 = texture(depthNormalTexture, f_texcoord[2]);\r\n"
			"vec4 sample3 = texture(depthNormalTexture, f_texcoord[3]);\r\n"
			"vec4 sample4 = texture(depthNormalTexture, f_texcoord[4]);\r\n"
			"float edge = 1.0f;\r\n"
			"edge *= CheckSame(sample1, sample2);\r\n"
			"edge *= CheckSame(sample3, sample4);\r\n"
			"edge = 1.0 - edge;\r\n"
			"vec4 withEdge = mix(texture(screenTexture, f_texcoord[0]), _EdgeColor, edge);\r\n"
			"// vec4 onlyEdge = mix(_BackgroundColor, _EdgeColor, edge);\r\n"
			"// vec3 col = mix(withEdge, onlyEdge, _EdgeOnly).rgb;\r\n"
			"// FragColor = vec4(col, 1.0);\r\n"
			"#define ONLY_DEPTH_NROMAL_RES\r\n"
			"#ifndef ONLY_DEPTH_NROMAL_RES\r\n"
			"FragColor = vec4(withEdge.rgb, 1.0);\r\n"
			"#else\r\n"
			"FragColor = vec4(texture(screenTexture, f_texcoord[0]).rgb,1.0);\r\n"
			"#endif\r\n"
			"}\r\n";

		static const char* v_image_effect = "#version 430\r\n"
			"layout(location = 0) in vec2 pos;\r\n"
			"layout(location = 1) in vec2 texcoord;\r\n"
			"out vec2 f_texcoord[5];\r\n"
			"float _SampleDistance = 1.0;\r\n"
			"uniform vec2 screenTexTexelSize;\r\n"
			"void main(){\r\n"
			"f_texcoord[0] = texcoord;\r\n"
			"f_texcoord[1] = texcoord + screenTexTexelSize.xy * vec2(1, 1) * _SampleDistance;\r\n"
			"f_texcoord[2] = texcoord + screenTexTexelSize.xy * vec2(-1, -1) * _SampleDistance;\r\n"
			"f_texcoord[3] = texcoord + screenTexTexelSize.xy * vec2(-1, 1) * _SampleDistance;\r\n"
			"f_texcoord[4] = texcoord + screenTexTexelSize.xy * vec2(1, -1) * _SampleDistance;\r\n"
			"gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\r\n"
			"}\r\n";
		static const char* f_normal_depth_write = "#version 430\r\n"
			"out vec4 FragColor;\r\n"
			"in vec3 f_normal;\r\n"
			"vec2 EncodeViewNormalStereo( vec3 n )\r\n"
			"{\r\n"
			"float kScale = 1.7777;\r\n"
			"vec2 enc;\r\n"
			"enc = n.xy / (n.z+1);\r\n"
			"enc /= kScale;\r\n"
			"enc = enc*0.5+0.5;\r\n"
			"return enc;\r\n"
			"}\r\n"
			"vec2 EncodeFloatRG( float v )\r\n"
			"{\r\n"
			"vec2 kEncodeMul = vec2(1.0, 255.0);\r\n"
			"float kEncodeBit = 1.0/255.0;\r\n"
			"vec2 enc = kEncodeMul * v;\r\n"
			"// enc.x = enc.x - int(enc.x);\r\n"
			"// enc.y = enc.y - int(enc.y);\r\n"
			"enc = fract(enc);\r\n"
			"enc.x -= enc.y * kEncodeBit;\r\n"
			"return enc;\r\n"
			"}\r\n"
			"void main(){\r\n"
			"FragColor = vec4(EncodeViewNormalStereo(f_normal), EncodeFloatRG(1 - gl_FragCoord.z));\r\n"
			"// float d = 1 - gl_FragCoord.z;\r\n"
			"// FragColor = vec4(d,d,d,1.0);\r\n"
			"// FragColor = vec4(1.0,1.0,1.0,1.0);\r\n"
			"}\r\n";

		static const char* v_normal_depth_write = "#version 430\r\n"
			"layout (location = 0) in vec3 pos;\r\n"
			"layout (location = 1) in vec3 normal;\r\n"
			"uniform mat3 t_inv_model;\r\n"
			"uniform mat3 mv;\r\n"
			"uniform mat4 mvp;\r\n"
			"out vec3 f_normal;\r\n"
			"void main()\r\n"
			"{\r\n"
			"gl_Position = mvp * vec4(pos, 1.0);\r\n"
			"// transform to [0, 1]\r\n"
			"f_normal = t_inv_model * normal;\r\n"
			"}\r\n";
		static const char* f_skybox = "#version 430 core\r\n"
			"out vec4 FragColor;\r\n"
			"in vec3 TexCoords;\r\n"
			"uniform samplerCube skybox;\r\n"
			"void main()\r\n"
			"{\r\n"
			"FragColor = texture(skybox, TexCoords);\r\n"
			"}\r\n";

		static const char* v_skybox = "#version 430 core\r\n"
			"layout (location = 0) in vec3 aPos;\r\n"
			"out vec3 TexCoords;\r\n"
			"uniform mat4 projection;\r\n"
			"uniform mat4 view;\r\n"
			"void main()\r\n"
			"{\r\n"
			"TexCoords = aPos;\r\n"
			"vec4 pos = projection * view * vec4(aPos, 1.0);\r\n"
			"gl_Position = pos.xyww;\r\n"
			"}\r\n";
		static const char* f_test = "#version 430\r\n"
			"layout(location = 0) in vec3 vGoColor;\r\n"
			"layout(location = 1) in vec3 vNormal;\r\n"
			"layout(location = 2) in vec3 vFragPos;\r\n"
			"layout(location = 3) flat in int vComp;\r\n"
			"layout(location = 4) in float vDistance;\r\n"
			"layout(location = 5) in float vDistanceM[6];\r\n"
			"layout(std140, binding = 1)uniform IFCRenderUBO{\r\n"
			"float alpha;			// 0 ~ 4\r\n"
			"int c_comp;				// 4 ~ 8\r\n"
			"int h_comp;				// 8 ~ 12\r\n"
			"vec3 cameraDirection;	// 16 ~ 32\r\n"
			"} ubo;\r\n"
			"layout(location = 0) out vec4 FragColor;\r\n"
			"void main()\r\n"
			"{\r\n"
			"for(int i=0;i<6;i++){\r\n"
			"if(vDistanceM[i]<0.0)\r\n"
			"discard;\r\n"
			"}\r\n"
			"if(vDistance<0.0)\r\n"
			"discard;\r\n"
			"vec3 color = vGoColor;\r\n"
			"if (ubo.h_comp == vComp) {\r\n"
			"color = vGoColor / 4 * 3 + vec3(0.0, 0.25, 0.0);\r\n"
			"}\r\n"
			"vec3 norm = normalize(vNormal);\r\n"
			"vec3 diffuse = max(dot(norm, ubo.cameraDirection), 0.0) * color * 0.3;\r\n"
			"color = color * 0.7 + diffuse;\r\n"
			"//FragColor = vec4(vNormal / 2 + 0.5,  ubo.alpha);\r\n"
			"FragColor =  vec4(color, ubo.alpha);\r\n"
			"}\r\n";

		static const char* v_test = "#version 430\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 1) in vec3 aNormal;\r\n"
			"layout(location = 2) in vec3 aColor;\r\n"
			"layout(location = 3) in int aComp;\r\n"
			"layout(std140, binding = 0)uniform TransformsUBO{\r\n"
			"mat4 view_model;				// 0 ~ 64\r\n"
			"mat4 proj_view_model;			// 64 ~ 128\r\n"
			"mat3 transpose_inv_model;		// 128 ~ 176\r\n"
			"vec4 uUserClipPlane;			// 176 ~ 192\r\n"
			"mat4 model;						// 192 ~ 256\r\n"
			"vec4 uUserClipBox[6];			// 256 ~ 352\r\n"
			"} ubo;\r\n"
			"layout(location = 0) out vec3 vGoColor;\r\n"
			"layout(location = 1) out vec3 vNormal;\r\n"
			"layout(location = 2) out vec3 vFragPos;\r\n"
			"layout(location = 3) flat out int vComp;\r\n"
			"layout(location = 4) out float vDistance;\r\n"
			"layout(location = 5) out float vDistanceM[6];\r\n"
			"void main()\r\n"
			"{\r\n"
			"vGoColor = aColor;\r\n"
			"vec4 p = vec4(aPos, 1.0);\r\n"
			"vec4 eyePos = ubo.model * p;\r\n"
			"vFragPos = eyePos.xyz;// 片段位置\r\n"
			"vDistance = dot(vFragPos, ubo.uUserClipPlane.xyz) - ubo.uUserClipPlane.w;\r\n"
			"for(int i=0;i<6;i++){\r\n"
			"vDistanceM[i]=dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w;\r\n"
			"//if(dot(eyePos.xyz, ubo.uUserClipBox[i].xyz) - ubo.uUserClipBox[i].w<0.0){\r\n"
			"//	vDistanceM= -1.0;\r\n"
			"//	break;\r\n"
			"//}\r\n"
			"}\r\n"
			"vComp = aComp;\r\n"
			"//vNormal = ubo.transpose_inv_model * aNormal;\r\n"
			"vNormal = aNormal;\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_text = "#version 430\r\n"
			"in vec2 TexCoords;\r\n"
			"out vec4 color;\r\n"
			"uniform sampler2D text;\r\n"
			"uniform vec3 textColor;\r\n"
			"void main()\r\n"
			"{\r\n"
			"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\r\n"
			"color = vec4(textColor, 1.0) * sampled;\r\n"
			"}\r\n";

		static const char* v_text = "#version 430\r\n"
			"layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\r\n"
			"out vec2 TexCoords;\r\n"
			"uniform vec2 offset;\r\n"
			"uniform mat4 projection;\r\n"
			"void main()\r\n"
			"{\r\n"
			"vec2 truepos = offset + vertex.xy;\r\n"
			"gl_Position = projection * vec4(truepos, 0.0, 1.0);\r\n"
			"TexCoords = vertex.zw;\r\n"
			"}\r\n";
		static const char* v_text3d = "#version 430\r\n"
			"layout(location = 0) in vec3 pos;\r\n"
			"layout(location = 1) in vec2 texcoord;\r\n"
			"out vec2 TexCoords;\r\n"
			"uniform mat4 projection;\r\n"
			"uniform mat4 modelview;\r\n"
			"void main()\r\n"
			"{\r\n"
			"gl_Position = projection * modelview * vec4(pos, 1.0);\r\n"
			"TexCoords = texcoord;\r\n"
			"}\r\n";
	}// shader_consts

}
#endif
