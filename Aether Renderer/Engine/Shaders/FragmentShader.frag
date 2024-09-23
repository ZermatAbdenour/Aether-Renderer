#version 330 core

in vec2 TexCoord;
in vec3 norm;
uniform sampler2D ourTexture;

uniform float far;
uniform float near;

float LinearizeDepth(){
    float ndc = gl_FragCoord.z*2-1;
    return (2.0 * near * far) / (far + near - ndc * (far - near));
}

void main()
{
    float depth = LinearizeDepth() / far;
    vec4 depthVec4 = vec4(vec3(pow(depth, 5)), 1.0);

    gl_FragColor = texture2D(ourTexture,TexCoord) * (1 - depthVec4) + depthVec4;
}