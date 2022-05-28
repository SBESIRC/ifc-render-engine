#pragma once
#ifndef IFCRE_NEW_SHADER_CONSTS_H_
#define IFCRE_NEW_SHADER_CONSTS_H_
namespace ifcre {
	namespace new_sc {
		static const char* f_axis = "#version 460\r\n"
			"layout(location = 0) flat in vec3 color;\r\n"
			"layout(location = 0) out vec4 FragColor;\r\n"
			"void main(){\r\n"
			"FragColor = vec4(color, 1.0);\r\n"
			"}\r\n";
		static const char* v_axis = "#version 460\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 0) flat out vec3 color;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"} ubo;\r\n"
			"void main(){\r\n"
			"int index = gl_VertexID >> 1;\r\n"
			"switch(index){\r\n"
			"case 0: color = vec3(1.0, 0.0, 0.0); break;\r\n"
			"case 1: color = vec3(0.0, 0.0, 1.0); break;\r\n"
			"case 2: color = vec3(0.0, 1.0, 0.0); break;\r\n"
			"default: color = vec3(1.0, 1.0, 1.0); break;\r\n"
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
		static const char* f_bbx = "#version 460\r\n"
			"out vec4 FragColor;\r\n"
			"void main()\r\n"
			"{\r\n"
			"FragColor = vec4(1.0, 0.6, 0.0, 1.0);\r\n"
			"}\r\n";

		static const char* v_bbx = "#version 460\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"uniform mat4 modelview;\r\n"
			"uniform mat4 projection;\r\n"
			"void main()\r\n"
			"{\r\n"
			"gl_Position = projection * modelview * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_comp_id_write = "#version 460\r\n"
			"layout(location = 0) out int FragId;\r\n"
			"layout(location = 0) flat in int vCompId;\r\n"
			"// vec3 int2rgb(int num){\r\n"
			"//     float b = (num % 256) / 256.;\r\n"
			"//     num = num >> 8;\r\n"
			"//     float g = (num % 256) / 256.;\r\n"
			"//     num = num >> 8;\r\n"
			"//     float r = (num % 256) / 256.;\r\n"
			"//     return vec3(r, g, b);\r\n"
			"// }\r\n"
			"void main(){\r\n"
			"// FragId=vec4(int2rgb(comp),1.);\r\n"
			"FragId = vCompId;\r\n"
			"}\r\n";

		static const char* v_comp_id_write = "#version 460\r\n"
			"layout (location = 0) in vec3 aPos;\r\n"
			"layout (location = 1) in vec3 aNormal;\r\n"
			"layout (location = 2) in vec3 aColor;\r\n"
			"layout (location = 3) in int aCompId;\r\n"
			"layout(std140, binding = 0)uniform TransformMVPUBO{\r\n"
			"mat4 proj_view_model;       // 0 ~ 64\r\n"
			"} ubo;\r\n"
			"layout (location = 0) flat out int vCompId;\r\n"
			"void main()\r\n"
			"{\r\n"
			"vCompId = aCompId;\r\n"
			"// gl_Position = mvp * vec4(pos, 1.0);\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_edge = "#version 460\r\n"
			"in vec3 go_color;\r\n"
			"out vec4 FragColor;\r\n"
			"void main()\r\n"
			"{\r\n"
			"FragColor = vec4(go_color,1.0);\r\n"
			"}\r\n";

		static const char* v_edge = "#version 460\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 2) in vec3 aColor;\r\n"
			"uniform mat4 mvp;\r\n"
			"out vec3 go_color;\r\n"
			"void main()\r\n"
			"{\r\n"
			"go_color = aColor/2;\r\n"
			"gl_Position = mvp * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_image_effect = "#version 460\r\n"
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

		static const char* v_image_effect = "#version 460\r\n"
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
		static const char* f_normal_depth_write = "#version 460\r\n"
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

		static const char* v_normal_depth_write = "#version 460\r\n"
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
		static const char* f_slct_bbx = "#version 460\r\n"
			"out vec4 FragColor;\r\n"
			"void main()\r\n"
			"{\r\n"
			"FragColor = vec4(1.0, 0.6, 0.0, 1.0);\r\n"
			"}\r\n";

		static const char* v_slct_bbx = "#version 460\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"uniform mat4 modelview;\r\n"
			"uniform mat4 projection;\r\n"
			"void main()\r\n"
			"{\r\n"
			"gl_Position = projection * modelview * vec4(aPos, 1.0);\r\n"
			"}\r\n";
		static const char* f_test = "#version 460\r\n"
			"layout(location = 0) in vec3 vGoColor;\r\n"
			"layout(location = 1) in vec3 vNormal;\r\n"
			"layout(location = 2) in vec3 vFragPos;\r\n"
			"layout(location = 3) flat in int vComp;\r\n"
			"layout(std140, binding = 1)uniform IFCRenderUBO{\r\n"
			"float alpha;			// 0 ~ 4\r\n"
			"int c_comp;				// 4 ~ 8\r\n"
			"int h_comp;				// 8 ~ 12\r\n"
			"vec3 cameraPos;			// 16 ~ 30\r\n"
			"} ubo;\r\n"
			"layout(location = 0) out vec4 FragColor;\r\n"
			"void main()\r\n"
			"{\r\n"
			"vec3 color = vGoColor;\r\n"
			"if(ubo.c_comp == vComp){\r\n"
			"color = vGoColor / 2 + vec3(0.0, 0.5, 0.0);\r\n"
			"}\r\n"
			"else if (ubo.h_comp == vComp) {\r\n"
			"color = vGoColor / 4 * 3 + vec3(0.0, 0.25, 0.0);\r\n"
			"}\r\n"
			"vec3 norm = normalize(vNormal);\r\n"
			"vec3 camdir = normalize(ubo.cameraPos - vFragPos);\r\n"
			"vec3 diffuse = max(dot(norm, camdir), 0.0) * color * 0.3;\r\n"
			"color = color * 0.7 + diffuse;\r\n"
			"FragColor = vec4(color, ubo.alpha);\r\n"
			"}\r\n";

		static const char* v_test = "#version 460\r\n"
			"layout(location = 0) in vec3 aPos;\r\n"
			"layout(location = 1) in vec3 aNormal;\r\n"
			"layout(location = 2) in vec3 aColor;\r\n"
			"layout(location = 3) in int aComp;\r\n"
			"layout(std140, binding = 0)uniform TransformsUBO{\r\n"
			"mat4 model;						// 0 ~ 64\r\n"
			"mat4 proj_view_model;			// 64 ~ 128\r\n"
			"mat3 transpose_inv_model;		// 128 ~ 176\r\n"
			"} ubo;\r\n"
			"layout(location = 0) out vec3 vGoColor;\r\n"
			"layout(location = 1) out vec3 vNormal;\r\n"
			"layout(location = 2) out vec3 vFragPos;\r\n"
			"layout(location = 3) flat out int vComp;\r\n"
			"void main()\r\n"
			"{\r\n"
			"vGoColor = aColor;\r\n"
			"vFragPos = vec3(ubo.model * vec4(aPos, 1.0));\r\n"
			"vComp = aComp;\r\n"
			"vNormal = ubo.transpose_inv_model * aNormal;\r\n"
			"gl_Position = ubo.proj_view_model * vec4(aPos, 1.0);\r\n"
			"}\r\n";
	}// shader_consts
}
#endif