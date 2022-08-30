#version 430

layout(location = 0) flat in int thisid;

layout(location = 0) out vec4 FragColor;

uniform int ui_id;

void main(){
    FragColor = vec4(0. ,1. ,1. , .2);
    if(ui_id == thisid){
        FragColor = vec4(0.5 ,.5 ,1. , 1.);
    }
}