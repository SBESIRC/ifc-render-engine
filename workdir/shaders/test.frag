#version 460
layout(location = 0) in vec4 vGoColor;
layout(location = 1) in vec3 vNormal;
in vec3 vFragPos;
layout(location = 3) flat in int vComp;
layout(location = 4) in float vDistance;
layout(location = 5) in float vDistanceM[7];
//layout(location = 6) in float dis_drawing_plane;

layout(std140, binding = 1)uniform IFCRenderUBO{
	float alpha;			// 0 ~ 4
	int c_comp;				// 4 ~ 8
	int h_comp;				// 8 ~ 12
	vec3 cameraDirection;	// 16 ~ 32
	vec3 cameraPosition;	// 32 ~ 48
} ubo;

layout(location = 0) out vec4 FragColor;

void main()
{
	for(int i=0;i<6;i++){
		if(vDistanceM[i]<0.0)
			discard;
	}
	if(vDistance<0.0)
		discard;
		
	if(vDistanceM[6] > 0.0) discard;
	
	vec4 color = vGoColor;
	if (ubo.h_comp == vComp) {
		color = vGoColor / 4 * 3 + vec4(0.0, 0.25, 0.0, 0.0);
    }
	vec3 norm = normalize(vNormal);
	vec4 diffuse = max(dot(norm, -ubo.cameraDirection), 0.0) * color * 0.3;

	vec4 tcolor = color * 0.7 + diffuse;
    FragColor = vec4(tcolor.xyz, color.a);
	//float k = dot(normalize(vFragPos - ubo.cameraPosition),norm);
	//if(k >.0 && color.w > 0.99) FragColor = vec4(1.0, 0.6, 0.0, 1.0);
}