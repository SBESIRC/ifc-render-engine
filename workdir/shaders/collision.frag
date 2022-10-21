#version 460
#define PI 3.14159265358979323846

layout(location = 0)in float vDistance;
layout(location = 1)in float vDistanceM[6];

uniform vec2 u_resolution;
uniform float alpha;

layout(location = 0)out vec4 FragColor;

vec2 rotate2D(vec2 _st,float _angle){
	_st-=0.5;
	_st = mat2(cos(_angle),-sin(_angle),
		sin(_angle),cos(_angle))*_st;
	_st+=0.5;
	return _st;
		
}

vec2 tile(vec2 _st, float _zoom){
	_st*=_zoom;
	return fract(_st);
}

void main()
{
	
	for(int i=0;i<6;i++){
		if(vDistanceM[i]<0.0)
			discard;
	}
	if(vDistance<0.0)
		discard;

	vec2 st = gl_FragCoord.xy/u_resolution;
	vec3 outColor=vec3(1.0,0.3729,0.4061);
	vec3 one=vec3(1.,1.,1.);

	st=tile(st,7.0);

	st=rotate2D(st,PI*0.25);

	float fin = mod(st.x,.1);
	fin=smoothstep(.04,.06,fin);
	outColor=mix(outColor,one ,fin*.5);

    FragColor = vec4(outColor, alpha);
}