#version 430
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 modelview;

void main()
{
    gl_Position = projection * modelview * vec4(pos, 1.0);
    TexCoords = texcoord;
}