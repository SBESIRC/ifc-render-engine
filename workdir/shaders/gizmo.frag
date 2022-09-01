#version 430

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord; // ������������

uniform sampler2D ourTexture; // ��һ��������ӵ�Ƭ����ɫ����

void main(){
    FragColor = texture(ourTexture,TexCoord); // �����������ɫ(���������,��Ӧ����������)
}