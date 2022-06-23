#version 460
layout(location = 0)in vec3 vGoColor;
layout(location = 1) flat in float vDistance;
layout(location = 0)out vec4 FragColor;
void main()
{
	if(vDistance<0.0)
		discard;
    FragColor = vec4(vGoColor,1.0);
}