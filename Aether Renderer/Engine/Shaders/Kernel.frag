#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
  
uniform sampler2D image;
uniform float kernel[9] = float[](
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);
    
uniform float offset = 1.0 / 3.0;
void main()
{             
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(image, TexCoord.st + offsets[i]));
    }
    vec3 result = vec3(0.0);
    for(int i = 0; i < 9; i++)
        result += sampleTex[i] * kernel[i];
    FragColor = vec4(result, 1.0);
}
