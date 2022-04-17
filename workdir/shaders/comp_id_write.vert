#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 3) in int aComp;//todo

uniform mat4 mvp;

out int comp;
void main()
{   
    comp= aComp;
    gl_Position = mvp * vec4(pos, 1.0);
}
