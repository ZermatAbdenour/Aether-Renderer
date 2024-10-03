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

vec3 ambiant = vec3(0.1);
uniform sampler2D diffuseMap;
float diffuse;
uniform sampler2D SpecularMap;
float shininess;

in vec3 worldPos;
out vec4 fragColor;


void main()
{   
    vec3 textureSample = texture(diffuseMap,fs_in.uv).xyz;
    vec3 ambiant = textureSample * ambiant;
    vec3 lightDirection = normalize(directionalLights[0].direction.xyz);
    vec3 normal = normalize(-fs_in.normal);
    float diff =dot(lightDirection,normal);
    vec3 diffuse = diff * directionalLights[0].color.xyz * textureSample; 
    vec3 viewDir = normalize(fs_in.camPos - worldPos);
    vec3 reflectDir = reflect(directionalLights[0].direction.xyz, normal);
    float spec = 0;
    if(diff>0)  
    spec = pow(max(dot(viewDir, reflectDir), 0.0), 10);
    vec3 specular =  spec * directionalLights[0].color.xyz *0.1f;  
    fragColor = vec4(ambiant + diffuse +specular,1);
}