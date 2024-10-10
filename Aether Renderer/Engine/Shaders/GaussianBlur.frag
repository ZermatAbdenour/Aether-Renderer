#version 330 core
out vec4 FragColor;
  
uniform sampler2DMS image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
in vec2 TexCoord;
void main()
{          
    ivec2 textureCoord = ivec2(gl_FragCoord);
    vec2 tex_offset = 1.0 / vec2(800,600);
    vec3 result = texelFetch(image, textureCoord,0).rgb * weight[0];
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texelFetch(image, textureCoord + ivec2(tex_offset.x * i, 0.0),0).rgb * weight[i];
            result += texelFetch(image, textureCoord - ivec2(tex_offset.x * i, 0.0),0).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texelFetch(image, textureCoord + ivec2(0.0, tex_offset.y * i),0).rgb * weight[i];
            result += texelFetch(image, textureCoord - ivec2(0.0, tex_offset.y * i),0).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}