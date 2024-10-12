#version 460 core

layout (location = 0) in vec3 postion;
layout (location = 2) in vec2 uv;

layout (std140,binding = 0) uniform Camera{
    mat4 projection;
    mat4 view;
    vec3 camPos;
};

uniform mat4 model;

void main(){
    gl_Position = projection * view * model * vec4(postion,1.0);
}