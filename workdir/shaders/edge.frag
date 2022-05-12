#version 460
in vec3 go_color;
out vec4 FragColor;
void main()
{
    FragColor = vec4(go_color,1.0);
}