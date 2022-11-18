#version 460
layout(location = 0) in vec3 aPos;
layout(location = 1) in float depth;

uniform mat4 view_matrix;

out float dep;
out vec3 dir;

void main(){
    dep = depth;
    dir = aPos;
    vec4 pos = view_matrix * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}