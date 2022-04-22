#version 450 core

out vec4 FragId;

flat in int comp;

void main(){
    FragId=vec4((comp>>16)/128.,(comp>>8)%128/128.,(comp%128)/128.,1.);
    //FragId=vec4(1.,1.,1.,1.);
}