#version 460
layout(location = 0)in vec3 vGoColor;
layout(location = 0)out vec4 FragColor;
void main()
{
    FragColor = vec4(vGoColor,1.0);
}