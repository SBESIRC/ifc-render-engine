#version 460

layout(location = 0) flat in int thisid;

layout(location = 0) out vec4 FragColor;

uniform int ui_id;
uniform vec3 this_color;

void main(){
    FragColor = vec4(this_color, .2);
    if(ui_id == thisid){
        FragColor = vec4(this_color / 2. , 1.);
    }
}