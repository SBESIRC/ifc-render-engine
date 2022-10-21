#version 460

layout(location = 0) out int FragId;

layout(location = 0) flat in int id;

void main(){
    FragId = id;
}