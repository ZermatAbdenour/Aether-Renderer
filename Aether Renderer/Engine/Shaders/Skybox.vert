#version 460 core

layout (location = 0) in vec3 position;

layout (std140,binding = 0) uniform Camera{
    mat4 projection;
    mat4 view;
    vec3 camPos;
};

out vec3 texCoord;

void main(){
    mat4 view = mat4(mat3(view));
    gl_Position = projection * view * vec4(position,1.0f);
    texCoord = position;
}