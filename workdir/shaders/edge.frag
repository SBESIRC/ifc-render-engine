#version 430
layout(location = 0)in vec3 vGoColor;
layout(location = 1)in float vDistance;
layout(location = 2)in float vDistanceM[6];

layout(location = 0)out vec4 FragColor;
void main()
{
	for(int i=0;i<6;i++){
		if(vDistanceM[i]<0.0)
			discard;
	}
	if(vDistance<0.0)
		discard;
    FragColor = vec4(vGoColor,1.0);
}