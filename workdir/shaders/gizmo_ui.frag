#version 430

layout(location = 0) out float FragId;

in float id;

void main(){
    FragId = id;
}