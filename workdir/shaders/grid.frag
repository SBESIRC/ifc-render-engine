#version 430

in vec3 lineColor;
in float lineType;

flat in vec3 startPos;
in vec3 vertPos;
uniform vec2  u_resolution;

layout(location = 0) out vec4 FragColor;

void main() {
    if(lineType < 0) {
	    vec2  dir  = (vertPos.xy-startPos.xy) * u_resolution/2.0;
        float dist = length(dir);

	    uint bit = uint(round(dist / 2.0f)) & 15U;
        if ((0x18ff & (1U<<bit)) == 0U)
            discard; 
    }
    FragColor = vec4(lineColor, 1.0);
}