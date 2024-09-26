#version 460 core

uniform sampler2D ourTexture;

uniform float far;
uniform float near;

in VS_OUT{
    vec2 uv;
    vec3 normal;
} fs_in;

out vec4 fragColor;
float LinearizeDepth(){
    float ndc = gl_FragDepth*2-1;
    return (2.0 * near * far) / (far + near - ndc * (far - near));
}

void main()
{
    float depth = LinearizeDepth() / far;
    vec4 depthVec4 = vec4(vec3(pow(depth, 5)), 1.0);


    fragColor = texture(ourTexture,fs_in.uv) * (1 - depthVec4) + depthVec4;
}