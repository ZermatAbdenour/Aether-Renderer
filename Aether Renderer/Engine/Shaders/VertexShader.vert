#version 460 core

layout(location = 0) in vec3 postion;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (std140,binding = 0) uniform Matrices{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;


out VS_OUT{
    vec2 uv;
    vec3 normal;
} vs_out;

void main(){

    gl_Position =  projection * view * model * vec4(postion,1.0);
    vs_out.uv = uv;
    vs_out.normal = normal;
}