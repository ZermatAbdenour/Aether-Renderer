#version 460 core

uniform sampler2D ssaoTexture;

in vec2 TexCoord;
out vec4 fragColor;
void main(){
    vec2 texelSize = 1.0/textureSize(ssaoTexture,0);
    vec3 color;
    for(int x= -2;x<2;x++){
        for(int y =-2;y<2;y++){
            vec2 offset = vec2(float(x),float(y))*texelSize;
            vec3 texColor = texture(ssaoTexture,TexCoord.xy+offset).xyz;
            color += texColor;
        }
    }
    color = color /(4*4);
    fragColor = vec4(color,1);
}