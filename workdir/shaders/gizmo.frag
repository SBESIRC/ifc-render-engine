#version 460

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;
in flat int thisid;

uniform sampler2D ourTexture;

void main(){
    vec4 tempColor = texture(ourTexture,TexCoord);
    if(thisid > 0){
        tempColor = tempColor*0.75+ vec4(0., 0.25, 0.25, 0.);
    }
    FragColor = tempColor;
}