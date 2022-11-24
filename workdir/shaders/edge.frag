#version 460
layout(location = 0)in vec4 vGoColor;
layout(location = 1)in float vDistance;
layout(location = 2)in float vDistanceM[7];

layout(location = 0)out vec4 FragColor;
void main()
{
	for(int i=0;i<6;i++){
		if(vDistanceM[i]<0.0)
			discard;
	}
	if(vDistance<0.0)
		discard;
	if(vDistanceM[6] > 0.0) discard;
	
    FragColor = vGoColor;
	//if(vDistanceM[6] > 0.0) FragColor = vec4(1.0, 0.6, 0.0, 1.0);
}