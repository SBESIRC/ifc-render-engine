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
		"struct Material"
		"{\n"
		"	vec4 kd;\n"
		"	vec4 ks;\n"
		"	float alpha;\n"
		"	int ns;\n"
		"};\n"
		"layout(std430, binding=0)in buffer Materials{\n"
		"	Material m[];\n"
		"};\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"out vec3 color;\n"
		"void main()\n"
		"{\n"
		"	color = aNormal * 0.5f + vec3(0.5f, 0.5f, 0.5f);\n"
		"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
		"}";

	const char* f_test = "#version 330 core\n"
		"in vec3 color;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(color, 1.0);\n"
		"}\n";
}

#endif