#version 460
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aColor;
uniform mat4 mvp;
out vec3 go_color;
void main()
{
	go_color = aColor/2;
	gl_Position = mvp * vec4(aPos, 1.0);
}