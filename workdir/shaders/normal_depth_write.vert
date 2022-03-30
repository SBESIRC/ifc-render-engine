#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat3 t_inv_model;
uniform mat4 mvp;

out vec3 f_normal;
out float f_depth;
void main()
{
    vec4 glPos = mvp * vec4(pos, 1.0);
    gl_Position = glPos;
    // transform to [0, 1]
    f_depth = (glPos.xyz / glPos.w).z;
    f_depth = f_depth * 0.5 + 0.5;
    f_depth = -f_depth + 1;
    
    f_normal = t_inv_model * normal;
}
