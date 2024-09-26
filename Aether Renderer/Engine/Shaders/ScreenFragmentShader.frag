#version 460 core

in vec2 TexCoord;

uniform sampler2D colorTexture;

out vec4 fragColor;
void main(){
    fragColor = texture(colorTexture,TexCoord);
}