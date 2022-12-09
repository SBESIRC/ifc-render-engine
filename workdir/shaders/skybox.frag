#version 460

#define PI 3.141592654
#define Eclipse (PI / 2.)
in float dep;
in vec3 dir;

layout(location = 0) out vec4 FragColor;
//uniform float Eclipse;

vec3 damp(float t, vec3 start, vec3 end){
    return start * (1 - t) + end * t;
}

float scaleOut(float t){
    if( t >= Eclipse)
        return (t - Eclipse)/( PI - Eclipse);
    return t / Eclipse;
}

vec2 cube2sphere(){
    vec3 direciton = normalize(dir);
    float phi = acos(direciton.y);
    float theta = acos(direciton.z / sin(phi));
    return vec2(theta, PI - phi);
}

void main(){
    vec3 outColor;
    vec2 thetaphi = cube2sphere();
    if(thetaphi.y >= Eclipse){
        //outColor = damp(scaleOut(thetaphi.y), vec3(0.8789, 0.9062, 0.9570), vec3(0.6015, 0.7070, 0.8906));
        outColor = damp(scaleOut(thetaphi.y), vec3(1., 1., 1.), vec3(1., 1., 1.));
    }
    else{
        //outColor = damp(scaleOut(thetaphi.y), vec3(0.5976, 0.3984, 0.1992), vec3(0.7890, 0.5781, 0.3671));
        outColor = damp(scaleOut(thetaphi.y), vec3(1., 1., 1.), vec3(1., 1., 1.));
    }
    FragColor = vec4(outColor, 1.0);
}