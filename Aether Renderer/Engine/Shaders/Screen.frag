#version 460 core

in vec2 TexCoord;

uniform sampler2DMS screenTexture;
uniform int samples;
out vec4 fragColor;


void main(){
    ivec2 textureCoord = ivec2(gl_FragCoord);
    vec4 antialiased;
    for(int i=0;i<samples;i++){
        vec4 sampleColor = texelFetch(screenTexture,textureCoord,i);
        antialiased += sampleColor /samples;
    }
    fragColor = antialiased;

}