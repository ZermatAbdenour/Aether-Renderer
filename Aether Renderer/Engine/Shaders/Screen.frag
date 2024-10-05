#version 460 core

in vec2 TexCoord;

uniform sampler2DMS screenTexture;
uniform int samples;
out vec4 fragColor;

const float gamma = 2.2f;

void main(){
    ivec2 textureCoord = ivec2(gl_FragCoord);
    vec3 antialiased;
    for(int i=0;i<samples;i++){
        vec4 sampleColor = texelFetch(screenTexture,textureCoord,i);
        antialiased += sampleColor.xyz /samples;
    }

    float exposure = 0.1f;
    vec3 toneMapped = vec3(1.0f) - exp(-antialiased *exposure);

    fragColor = vec4(pow(toneMapped,vec3(1)/gamma),1);
    //fragColor = vec4(antialiased);

}