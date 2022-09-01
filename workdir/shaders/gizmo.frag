#version 430

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord; // 传入纹理坐标

uniform sampler2D ourTexture; // 把一个纹理添加到片段着色器中

void main(){
    FragColor = texture(ourTexture,TexCoord); // 采样纹理的颜色(纹理采样器,对应的纹理坐标)
}