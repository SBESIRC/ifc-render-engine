#version 460
layout(location = 0) in vec4 vGoColor;
layout(location = 1) in vec3 vNormal;
in vec3 vFragPos;
layout(location = 3) flat in int vComp;
layout(location = 4) in float vDistance;
layout(location = 5) in float vDistanceM[6];
in float dis_drawing_plane;

layout(std140, binding = 1)uniform IFCRenderUBO{
	float alpha;			// 0 ~ 4
	int c_comp;				// 4 ~ 8
	int h_comp;				// 8 ~ 12
	vec3 cameraDirection;	// 16 ~ 32
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
	if(dis_drawing_plane > 0.0) discard;
	vec3 color = vGoColor.rgb / 2.0 + vec3(0.0, 0.5, 0.0);
	if (ubo.h_comp == vComp) {
		color = vGoColor.rgb / 4.0 * 3.0 + vec3(0.0, 0.25, 0.0);
    }
	vec3 norm = normalize(vNormal);
	//vec4 colorr = vec4(color.r, color.g, color.b, vGoColor.a);
	vec3 diffuse = max(dot(norm, ubo.cameraDirection), 0.0) * color * 0.3;

	color = color * 0.7 + diffuse;
    //FragColor = vec4(color, ubo.alpha);
	FragColor = vec4(color, vGoColor.a);
}