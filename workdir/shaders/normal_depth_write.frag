#version 450 core
out vec4 FragColor;

in vec3 f_normal;
in float f_depth;

vec2 EncodeViewNormalStereo( vec3 n )
{
    float kScale = 1.7777;
    vec2 enc;
    enc = n.xy / (n.z+1);
    enc /= kScale;
    enc = enc*0.5+0.5;
    return enc;
}

vec2 EncodeFloatRG( float v )
{
    vec2 kEncodeMul = vec2(1.0, 255.0);
    float kEncodeBit = 1.0/255.0;
    vec2 enc = kEncodeMul * v;
    // enc.x = enc.x - int(enc.x);
    // enc.y = enc.y - int(enc.y);
    enc = fract(enc);
    enc.x -= enc.y * kEncodeBit;
    return enc;
}

void main(){
    FragColor = vec4(EncodeViewNormalStereo(f_normal), EncodeFloatRG(f_depth));
    // FragColor = vec4(1.0,1.0,1.0,1.0);
}