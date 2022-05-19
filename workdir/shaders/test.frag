#version 460
layout(location = 0) in vec3 vGoColor;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vFragPos;
layout(location = 3) flat in int vComp;

layout(std140, binding = 1)uniform IFCRenderUBO{
	float alpha;			// 0 ~ 4
	int c_comp;				// 4 ~ 8
	int h_comp;				// 8 ~ 12
	vec3 cameraPos;			// 16 ~ 30
} ubo;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec3 color = vGoColor;
    if(ubo.c_comp == vComp){
        color = vGoColor / 2 + vec3(0.0, 0.5, 0.0);
	}
	else if (ubo.h_comp == vComp) {
		color = vGoColor / 4 * 3 + vec3(0.0, 0.25, 0.0);
    }
	vec3 norm = normalize(vNormal);
	vec3 camdir = normalize(ubo.cameraPos - vFragPos);
	vec3 diffuse = max(dot(norm, camdir), 0.0) * color * 0.3;
    FragColor = vec4(color * 0.7 + diffuse, ubo.alpha);
	// FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}