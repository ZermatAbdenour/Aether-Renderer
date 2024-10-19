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
in VS_OUT{
    vec2 uv;
    vec3 camPos;
    mat3 TBN;
    vec3 fragPos;
    vec4 fragPosClipSpace;
    vec4 fragPosLightSpace;
} fs_in;

vec3 ambiantValue = vec3(0.05);
uniform sampler2D diffuseMap;
uniform vec4 baseColor;
uniform bool baseColorOnly;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D occlusionTexture;
uniform sampler2D shadowMap;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;

//settings
uniform bool ssao;
uniform bool SSAOOnly;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = 0.00005;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}  

void main()
{
    vec2 NDCSpaceFragPos = fs_in.fragPosClipSpace.xy / fs_in.fragPosClipSpace.w;
    vec2 ssaoTextureLookup = NDCSpaceFragPos * 0.5 + 0.5;

    vec3 normal = texture(normalMap,fs_in.uv).rgb * 2-1;
    normal = normalize(fs_in.TBN * normal);
    float AmbientOcclusion = texture(occlusionTexture, ssaoTextureLookup).r ;
    vec3 textureSample;
    if(baseColorOnly)
        textureSample = baseColor.xyz;
    else
        textureSample = texture(diffuseMap,fs_in.uv).xyz * baseColor.xyz;

    vec3 ambiant;
    if(SSAOOnly){
        fragColor = vec4(vec3(AmbientOcclusion),1);
        return;
    }

    if(ssao)
        ambiant = textureSample * ambiantValue * AmbientOcclusion;
    else
        ambiant = textureSample * ambiantValue;


    vec3 lightDirection = normalize(directionalLights[0].direction.xyz);

    float diff =max(dot(lightDirection,-normal),0.0);
    vec3 diffuse = diff * directionalLights[0].color.xyz * textureSample; 
    vec3 viewDir = normalize(fs_in.camPos - fs_in.fragPos);
    vec3 reflectDir = reflect(directionalLights[0].direction.xyz, -normal);
    float spec = 0;
    vec3 specular = vec3(0);
    if(diff>0)  {
        spec = pow(max(dot(viewDir, reflectDir), 0.01), 36);
        specular = spec * directionalLights[0].color.xyz *texture(specularMap,fs_in.uv).xyz;  
    }
    float shadow = ShadowCalculation(fs_in.fragPosLightSpace);

    fragColor = vec4(vec3(ambiant+(1-shadow)*diffuse+ specular),1);
    
    //Calculate bloom color
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.5)
        bloomColor = vec4(fragColor.rgb, 1.0);
    else
        bloomColor = vec4(0.0, 0.0, 0.0, 1.0);

}