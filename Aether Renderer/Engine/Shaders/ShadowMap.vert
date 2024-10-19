#version 460 core

layout (location = 0) in vec3 postion;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main(){
    gl_Position = lightSpaceMatrix * model * vec4(postion,1);  
}