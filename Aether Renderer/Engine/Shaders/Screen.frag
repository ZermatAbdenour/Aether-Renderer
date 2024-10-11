#version 460 core

in vec2 TexCoord;

uniform sampler2DMS MSScreenTexture;
uniform sampler2D screenTexture;
uniform bool multiSampling;
uniform int samples;
//Settings
uniform bool toneMapping;
uniform float exposure;
uniform bool gammaCorrection;
uniform float gamma;

out vec4 fragColor;

void main(){

    vec3 color;
    //Antialiasing
    if(multiSampling){
        ivec2 textureCoord = ivec2(gl_FragCoord);
        for(int i=0;i<samples;i++){
            vec3 sampleColor = texelFetch(MSScreenTexture,textureCoord,i).rgb;
            color += sampleColor.xyz /samples;
        }
    }
    else{
        color = texture(screenTexture,TexCoord).rgb;
        fragColor = vec4(color,1);
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