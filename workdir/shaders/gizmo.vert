#version 460
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 uvs;
layout(location = 2) in int aid;
layout(std140, binding = 0)uniform TransformMVPUBO{
    mat4 view_matrix;   // 0 ~ 64
} ubo;

uniform int hover_id;

out vec2 TexCoord;
out flat int thisid;

void main(){
    TexCoord = uvs;
    gl_Position = ubo.view_matrix * vec4(aPos, 1.0) + vec4(.9, .8, 0., 0.);
    thisid = 0;
    if(hover_id == aid)
        thisid = 1;
}