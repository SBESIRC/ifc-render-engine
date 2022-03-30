#version 450 core
out vec4 FragColor;
in vec2 f_texcoord;
uniform sampler2D screenTexture;
uniform sampler2D depthNormalTexture;

void main(){
    vec3 col = texture(screenTexture, f_texcoord).rgb;
    FragColor = vec4(col, 1.0);
}