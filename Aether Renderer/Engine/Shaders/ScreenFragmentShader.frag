#version 330 core

in vec2 TexCoord;

uniform sampler2D colorTexture;

void main(){
    gl_FragColor = texture2D(colorTexture,TexCoord);
    //gl_FragColor = vec4(0.5f);
}