#version 460 core

layout (location = 0) in vec3 postion;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent; 

layout (std140,binding = 0) uniform Camera{
    mat4 projection;
    mat4 view;
    vec3 camPos;
};

uniform mat4 model;

out vec3 worldPos;
out VS_OUT{
    vec2 uv;
    vec3 camPos;
    mat3 TBN;
    vec4 clipSpaceFragPos;
} vs_out;

void main(){

    worldPos = vec3(model * vec4(postion,1.0f));
    
    vs_out.uv = uv;
    vs_out.camPos = camPos;
    vs_out.TBN = mat3(model) *mat3(tangent,bitangent,normal); 

    gl_Position = projection * view * model * vec4(postion,1.0);
    vs_out.clipSpaceFragPos = gl_Position;
}