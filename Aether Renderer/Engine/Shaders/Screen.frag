#version 460 core

in vec2 TexCoord;

uniform sampler2DMS screenTexture;
uniform int samples;

//Settings
uniform bool toneMapping;
uniform float exposure;
uniform bool gammaCorrection;
uniform float gamma;

out vec4 fragColor;

void main(){
    ivec2 textureCoord = ivec2(gl_FragCoord);

    vec3 color;
    //Antialiasing
    for(int i=0;i<samples;i++){
        vec4 sampleColor = texelFetch(screenTexture,textureCoord,i);
        color += sampleColor.xyz /samples;
    }

    //Tonemapping
    if(toneMapping)
        color = vec3(1.0f) - exp(-color *exposure);

    //Gamma Correction
    if(gammaCorrection)
        color = pow(color,vec3(1)/gamma);

    fragColor = vec4(color,1);
   /// fragColor = vec4(antialiased,1);
}