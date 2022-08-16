#version 430
out vec4 FragColor;

in vec2 f_texcoord[5];

uniform sampler2D screenTexture;
uniform sampler2D depthNormalTexture;

const vec2 _Sensitivity = vec2(1.0, 1.0);
const vec4 _EdgeColor = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 _BackgroundColor = vec4(1.0, 1.0, 1.0, 1.0);
const float _EdgeOnly = 0.5f;

float DecodeFloatRG(in vec2 enc)
{
    vec2 kDecodeDot = vec2(1.0, 1/255.0);
    return dot( enc, kDecodeDot );
}

float CheckSame(in vec4 col1,in vec4 col2){
    float d1 = DecodeFloatRG(col1.zw);
	vec2 normal1 = col1.xy;
	float d2 = DecodeFloatRG(col2.zw);
	vec2 normal2 = col2.xy;

	vec2 diffNormal = abs(normal2 - normal1) * _Sensitivity.x;
	float diffDepth = abs(d2 - d1) * _Sensitivity.y;
	int isSameNormal = (diffNormal.x + diffNormal.y) < 0.1?1:0;
	int isSameDepth = (diffDepth < (0.1 * d1)) ? 1:0;

	return (isSameDepth * isSameNormal != 0) ? 1.0 : 0.0;
}

void main(){
    vec4 sample1 = texture(depthNormalTexture, f_texcoord[1]);
	vec4 sample2 = texture(depthNormalTexture, f_texcoord[2]);
	vec4 sample3 = texture(depthNormalTexture, f_texcoord[3]);
	vec4 sample4 = texture(depthNormalTexture, f_texcoord[4]);

	float edge = 1.0f;
	edge *= CheckSame(sample1, sample2);
	edge *= CheckSame(sample3, sample4);

	edge = 1.0 - edge;

	vec4 withEdge = mix(texture(screenTexture, f_texcoord[0]), _EdgeColor, edge);
	// vec4 onlyEdge = mix(_BackgroundColor, _EdgeColor, edge);
	// vec3 col = mix(withEdge, onlyEdge, _EdgeOnly).rgb;
    // FragColor = vec4(col, 1.0);
#define ONLY_DEPTH_NROMAL_RES
#ifndef ONLY_DEPTH_NROMAL_RES
    FragColor = vec4(withEdge.rgb, 1.0);
#else
    FragColor = vec4(texture(screenTexture, f_texcoord[0]).rgb,1.0);
#endif
}