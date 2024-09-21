#version 330 core

layout(location = 0) in vec3 postion;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec2 TexCoord;

void main(){
    gl_Position =  u_projection * u_view * u_model * vec4(postion,1.0);
    TexCoord = aTexCoord;
}