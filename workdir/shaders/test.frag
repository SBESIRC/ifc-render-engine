#version 330 core
in vec3 go_color;
in vec3 normal;
in vec3 FragPos;
flat in int comp;
uniform float alpha;
uniform vec3 cameraPos;
uniform int c_comp;
uniform int h_comp;
out vec4 FragColor;
void main()
{
    vec3 color = go_color;
    if(c_comp==comp){
        color = go_color / 2 + vec3(0.0,0.5,0.0);
	}
	else if (h_comp == comp) {
		color = go_color / 4 * 3 + vec3(0.0, 0.25, 0.0);
    }
	vec3 norm = normalize(normal);
	vec3 camdir = normalize(cameraPos - FragPos);
	vec3 diffuse = max(dot(norm, camdir), 0.0) * color * 0.3;
    FragColor = vec4(color * 0.7 + diffuse, alpha);
}