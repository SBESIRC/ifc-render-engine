#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in int aComp;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 go_color;
out vec3 normal;
out vec3 FragPos;
flat out int comp;
void main()
{
	go_color = aColor;
	FragPos = vec3(model * vec4(aPos, 1.0));
	comp = aComp;
	normal = mat3(transpose(inverse(model)))*aNormal;
	gl_Position = projection * view * vec4(FragPos, 1.0);
}