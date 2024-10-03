#version 460 core

const float PI = 3.14159265359;
const int MAX_POINTLIGHTS =10;
const int MAX_DIRECTIONALLIGHTS =5;


struct PointLight{
    vec4 position;
    vec4 direction;
    vec4 color;
};
struct DirectionalLight{
    vec4 direction;
    vec4 color;
};
layout(std140,binding = 1) uniform Lights{
    int numDirectionalLight;
    int numPointLight;
    DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
    PointLight pointLights[MAX_POINTLIGHTS];
};
uniform sampler2D ourTexture;

in VS_OUT{
    vec2 uv;
    vec3 normal;
    vec3 camPos;
} fs_in;

uniform sampler2D diffuseMap;
float diffuse;
uniform sampler2D SpecularMap;
float shininess;

in vec3 worldPos;
out vec4 fragColor;


void main()
{   
    //vec3 albedo = texture(diffuseMap, fs_in.uv).rgb; 
    vec3 albedo;
    for(int i =0;i<numDirectionalLight;++i){
        vec3 dirLight = directionalLights[i].color.xyz;
        albedo += dirLight;
    }
    fragColor = vec4(albedo,1);
}