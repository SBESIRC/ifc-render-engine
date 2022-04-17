#version 450 core

out vec4 FragId;

flat in int comp;

void main(){
    FragId=vec4((64*comp>>16)/255.,(64*comp>>8)%128/255.,(64*comp%128)/255.,1.);
    //FragId=vec4(1.,1.,1.,1.);
}
