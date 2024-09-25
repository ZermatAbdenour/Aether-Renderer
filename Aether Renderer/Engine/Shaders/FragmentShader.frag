#version 330 core

uniform sampler2D ourTexture;

uniform float far;
uniform float near;

in VS_OUT{
    vec2 uv;
    vec3 normal;
} fs_in;


float LinearizeDepth(){
    float ndc = gl_FragDepth*2-1;
    return (2.0 * near * far) / (far + near - ndc * (far - near));
}

void main()
{
    float depth = LinearizeDepth() / far;
    vec4 depthVec4 = vec4(vec3(pow(depth, 5)), 1.0);

    if(gl_FragCoord.x > 350 && gl_FragCoord.x<450 &&gl_FragCoord.y > 250 && gl_FragCoord.y<350 &&  depth<1)
        discard;

    gl_FragColor = texture2D(ourTexture,fs_in.uv) * (1 - depthVec4) + depthVec4;
}