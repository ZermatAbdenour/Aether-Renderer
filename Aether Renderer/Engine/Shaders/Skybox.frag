#version 460 core

uniform samplerCube cubeMap;

in vec3 texCoord;

out vec4 fragColor;
void main(){
    fragColor = texture(cubeMap,texCoord);
}