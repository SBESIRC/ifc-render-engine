#version 460

out vec4 FragId;

flat in int comp;

vec3 int2rgb(int num){
    float b = (num % 256) / 256.;
    num = num >> 8;
    float g = (num % 256) / 256.;
    num = num >> 8;
    float r = (num % 256) / 256.;
    return vec3(r, g, b);
}

void main(){
    FragId=vec4(int2rgb(comp),1.);
    //FragId=vec4((comp>>16)/128.,(comp>>8)%128/128.,(comp%128)/128.,1.);
    //FragId=vec4(1.,1.,1.,1.);
}
