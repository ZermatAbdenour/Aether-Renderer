#version 460 core

const float PI = 3.14159265359;

uniform sampler2D ourTexture;

in VS_OUT{
    vec2 uv;
    vec3 normal;
    vec3 camPos;
} fs_in;

uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

in vec3 worldPos;
out vec4 fragColor;


void main()
{   
    vec3 albedo = texture(albedoMap, fs_in.uv).rgb;
    fragColor = vec4(albedo,1.0f);
    return;

}