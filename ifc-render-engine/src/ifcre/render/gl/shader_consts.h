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
		"//layout(location = 3) in int aComp;\n"
		"uniform mat4 modelview;\n"
		"uniform mat4 projection;\n"
		"out vec3 color;\n"
		"//out int comp;\n"
		"void main()\n"
		"{\n"
		"	color = aColor*0.7f+aNormal*0.3f;\n"
		"   //comp = aComp;\n"
		"	gl_Position = projection * modelview * vec4(aPos, 1.0);\n"
		"}";

	const char* f_test = "#version 330 core\n"
		"in vec3 color;\n"
		"in int comp;\n"
		"uniform float alpha;\n"
		"//uniform int c_comp;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   //if(c_comp==comp)\n"
		"   //   color=color/2+vec3(0.0,0.5,0.0);\n"
		"	FragColor = vec4(color, alpha);\n"
		"}\n";

	
}

#endif