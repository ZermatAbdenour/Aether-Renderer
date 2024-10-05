#version 460 core

const float gamma = 2.2f;

in vec2 TexCoord;

uniform sampler2DMS screenTexture;
uniform int samples;
uniform float sceneExposure;

out vec4 fragColor;
void main(){
    ivec2 textureCoord = ivec2(gl_FragCoord);
    vec3 antialiased;
    for(int i=0;i<samples;i++){
        vec4 sampleColor = texelFetch(screenTexture,textureCoord,i);
        antialiased += sampleColor.xyz /samples;
    }

    vec3 toneMapped = vec3(1.0f) - exp(-antialiased *sceneExposure);

    fragColor = vec4(pow(toneMapped,vec3(1)/gamma),1);
   /// fragColor = vec4(antialiased,1);
}