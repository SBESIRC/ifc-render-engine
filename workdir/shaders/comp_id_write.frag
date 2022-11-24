#version 460

layout(location = 0) out int FragId;

layout(location = 0) flat in int vCompId;
layout(location = 1) in float vDistance;
layout(location = 2) in float vDistanceM[7];

// vec3 int2rgb(int num){
//     float b = (num % 256) / 256.;
//     num = num >> 8;
//     float g = (num % 256) / 256.;
//     num = num >> 8;
//     float r = (num % 256) / 256.;
//     return vec3(r, g, b);
// }

void main(){
    for(int i=0;i<6;i++){
		if(vDistanceM[i]<0.0)
			discard;
	}
    if(vDistance<0.0)
		  //FragId=-1;
          discard;
    // FragId=vec4(int2rgb(comp),1.);
    if(vDistanceM[6] > 0.03) discard;
    FragId = vCompId;
}
