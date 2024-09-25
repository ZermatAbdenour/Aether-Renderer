#version 330 core

layout(location = 0) in vec3 postion;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 u_model;


mat4 u_view;
mat4 u_projection;


out VS_OUT{
    vec2 uv;
    vec3 normal;
} vs_out;

void main(){
    gl_Position =  u_projection * u_view * u_model * vec4(postion,1.0);
    vs_out.uv = uv;
    vs_out.normal = normal;
}