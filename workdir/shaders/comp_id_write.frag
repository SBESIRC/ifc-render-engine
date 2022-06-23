#version 460

layout(location = 0) out int FragId;

layout(location = 0) flat in int vCompId;
layout(location = 1) flat in float vDistance;

// vec3 int2rgb(int num){
//     float b = (num % 256) / 256.;
//     num = num >> 8;
//     float g = (num % 256) / 256.;
//     num = num >> 8;
//     float r = (num % 256) / 256.;
//     return vec3(r, g, b);
// }

void main(){
    // FragId=vec4(int2rgb(comp),1.);
    FragId = vCompId;
    if(vDistance<0.0)
		FragId=-1;
}
