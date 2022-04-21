#pragma once
#ifndef IFCRE_SHADER_CONSTS_H_
#define IFCRE_SHADER_CONSTS_H_

namespace ifcre {
	const char* v_offscreen = "#version 330 core\n"
		"layout(location = 0) in vec2 aPos; \n"
		"layout(location = 1) in vec2 aTexCoords;\n"
		"out vec2 TexCoords;\n"
		"void main()\n"
		"{"
		"	TexCoords = aTexCoords;\n"
		"	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
		"}  ";

	const char* f_offscreen = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoords;\n"
		"uniform sampler2D screenTexture;\n"
		"void main()\n"
		"{\n"
		"	vec3 col = texture(screenTexture, TexCoords).rgb;\n"
		"	FragColor = vec4(col, 1.0);\n"
		"} ";

	const char* v_test = "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aNormal;\n"
		"layout(location = 2) in vec3 aColor;\n"
		"layout(location = 3) in int aComp;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"out vec3 go_color;\n"
		"out vec3 normal;\n"
		"out vec3 FragPos;\n"
		"flat out int comp;\n"
		"void main()\n"
		"{\n"
		"	go_color = aColor;\n"
		"	FragPos = vec3(model * vec4(aPos, 1.0));\n"
		"   comp = aComp;\n"
		"   normal = mat3(transpose(inverse(model)))*aNormal;\n"
		"	gl_Position = projection * view * vec4(FragPos, 1.0);\n"
		"}";

	const char* f_test = "#version 330 core\n"
		"in vec3 go_color;\n"
		"in vec3 normal;\n"
		"in vec3 FragPos;\n"
		"flat in int comp;\n"
		"uniform float alpha;\n"
		"uniform vec3 cameraPos;\n"
		"uniform int c_comp;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   vec3 color = go_color;"
		"   if(c_comp==comp){\n"
		"      color = go_color / 2 + vec3(0.0,0.5,0.0);\n"
		"   }\n"
		"   vec3 norm = normalize(normal);\n"
		"   vec3 camdir = normalize(cameraPos - FragPos);\n"
		"   vec3 diffuse = max(dot(norm, camdir), 0.0) * color * 0.3;\n"
		"	FragColor = vec4(color * 0.7 + diffuse, alpha);\n"
		"}\n";

	
}

#endif