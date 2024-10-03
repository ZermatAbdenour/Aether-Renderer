#version 460 core

layout(location = 0) in vec3 postion;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (std140,binding = 0) uniform Camera{
    mat4 projection;
    mat4 view;
    vec3 camPos;
};

uniform mat4 model;

out vec3 worldPos;
out VS_OUT{
    vec2 uv;
    vec3 normal;
    vec3 camPos;
} vs_out;

void main(){

    
    worldPos = vec3(model * vec4(postion,1.0f));
    vec3 outnormal = mat3(transpose(inverse(model))) * normal;  

    vs_out.uv = uv;
    vs_out.normal = outnormal;
    vs_out.camPos = camPos;
    gl_Position = projection * view * model * vec4(postion,1.0);
}