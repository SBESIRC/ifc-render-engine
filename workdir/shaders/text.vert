#version 460
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform vec2 offset;
uniform mat4 projection;

void main()
{
    vec2 truepos = offset + vertex.xy;
    gl_Position = projection * vec4(truepos, 0.0, 1.0);
    TexCoords = vertex.zw;
}  