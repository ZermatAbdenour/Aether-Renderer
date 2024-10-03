#version 460 core

in vec2 TexCoord;

uniform sampler2DMS screenTexture;
uniform int samples;
out vec4 fragColor;

const float gamma = 2.2f;

void main(){
    ivec2 textureCoord = ivec2(gl_FragCoord);
    vec4 antialiased;
    for(int i=0;i<samples;i++){
        vec4 sampleColor = texelFetch(screenTexture,textureCoord,i);
        antialiased += sampleColor /samples;
    }


    fragColor = vec4(pow(antialiased.xyz,vec3(1)/gamma),1);
    //fragColor = vec4(antialiased);

}