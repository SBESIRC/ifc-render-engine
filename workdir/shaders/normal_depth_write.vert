#version 430
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat3 t_inv_model;
uniform mat3 mv;
uniform mat4 mvp;

out vec3 f_normal;
void main()
{   
    gl_Position = mvp * vec4(pos, 1.0);
    // transform to [0, 1]
    f_normal = t_inv_model * normal;
}
