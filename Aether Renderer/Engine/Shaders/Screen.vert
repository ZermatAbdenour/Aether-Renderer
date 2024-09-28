#version 460 core

layout(location = 0) in vec3 postion;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec2 TexCoord;

void main(){
    gl_Position = vec4(postion * 2,1.0);
    TexCoord = uv;
}