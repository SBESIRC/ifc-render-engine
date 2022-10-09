#version 430

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord; // 传入纹理坐标
in flat int thisid;

uniform sampler2D ourTexture; // 把一个纹理添加到片段着色器中

void main(){
    vec4 tempColor = texture(ourTexture,TexCoord); // 采样纹理的颜色(纹理采样器,对应的纹理坐标)
    if(thisid > 0){
        tempColor = tempColor*0.75+ vec4(0., 0.25, 0.25, 0.);
    }
    FragColor = tempColor;
}