#version 460 core

const float PI = 3.14159265359;
const vec3 F0 = vec3(0.04); 
const int MAX_POINTLIGHTS =10;
const int MAX_DIRECTIONALLIGHTS =5;
struct PointLight{
    vec4 position;
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

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D metalicMap;
uniform sampler2D ssaoTexture;
uniform sampler2D shadowMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;  

uniform bool baseColorOnly;
uniform vec4  baseColor;
uniform float metallic;
uniform float roughness;
uniform float ao;

//Settings
uniform bool SSAO;
uniform bool SSAOOnly;
uniform bool shadowMapping;
uniform bool softShadow;
uniform float bias ;
uniform float minBias;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;



vec3 fresnelSchlick(float cosTheta, vec3 F0); 
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);  
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

vec3 CalculateDirectionalLight(DirectionalLight directionalLight,vec3 V,vec3 albedo,vec3 normal,float metallic,float roughness,bool shadowMapTarget);
vec3 CalculatePointLight(PointLight pointLight,vec3 V,vec3 albedo,vec3 normal,float metallic,float roughness);
float CalculateShadow(vec4 fragPosLightSpace,vec3 normal,vec3 lightDir);

void main()
{   
    vec3 albedo = vec3(0);
    float metallic = texture(metalicMap,fs_in.uv).b;
    float roughness = texture(metalicMap,fs_in.uv).g;
    float ao = texture(metalicMap,fs_in.uv).a;
    
    vec2 NDCSpaceFragPos = fs_in.fragPosClipSpace.xy / fs_in.fragPosClipSpace.w;
    vec2 ssaoTextureLookup = NDCSpaceFragPos * 0.5 + 0.5;
    float ssao = texture(ssaoTexture,ssaoTextureLookup).r;

    if(SSAOOnly)
    {
        fragColor = vec4(vec3(ssao),1);
        return;
    }


    if(baseColorOnly)
        albedo = baseColor.xyz;
    else
        albedo = texture(diffuseMap,fs_in.uv).xyz * baseColor.xyz;

    vec3 normal = texture(normalMap,fs_in.uv).rgb * 2-1;
    normal = normalize(fs_in.TBN * normal);
    vec3 V = normalize(fs_in.camPos - fs_in.fragPos);

    
    vec3 Lo = vec3(0);
    //Calculate DirectionalLights contribution
    for(int i =0;i<numDirectionalLight;i++){
        Lo += CalculateDirectionalLight(directionalLights[i],V,albedo,normal,metallic,roughness,i==0);
    }
    //Calculate PointLights contribution
    for(int i = 0;i<numPointLight;i++){
        Lo += CalculatePointLight(pointLights[i],V,albedo,normal,metallic,roughness);
    }
    vec3 F = fresnelSchlickRoughness(max(dot(normal,V),0.0f),F0,roughness);
    vec3 KS = F;
    vec3 KD = 1-KS;
    vec3 irradiance = texture(irradianceMap,normal).rgb;
    vec3 diffuse    = irradiance * albedo;

    vec3 R = reflect(-V, normal);   
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;   
    //fragColor = vec4(prefilteredColor,1);
    //return;
    vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(normal, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
  
    vec3 ambient;
    if(SSAO)
        ambient = (KD * diffuse + specular) * ao * ssao;
    else
        ambient = (KD * diffuse + specular) * ao;
    vec3 color = ambient + Lo;
	
    fragColor = vec4(color,1.0);

    //Calculate bloom color
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.5)
        bloomColor = vec4(fragColor.rgb, 1.0);
    else
        bloomColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
}

float CalculateShadow(vec4 fragPosLightSpace,vec3 normal,vec3 lightDir)
{
    lightDir = normalize(lightDir);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
     
    float currentDepth = projCoords.z;
    float bias = max(bias * (1 - dot(normal, lightDir)), minBias);  

    float shadow = 0;
    if(softShadow){
        //Percentage closer filtering
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
            }    
        }
        shadow /= 9.0;
    }
    else//Hard shadows (no filtering)
    {
        float currentShadow = float(currentDepth > closestDepth);
        shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
    }
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow ;
}  

vec3 CalculateDirectionalLight(DirectionalLight directionalLight,vec3 V,vec3 albedo,vec3 normal,float metallic,float roughness,bool shadowMapTarget){
        vec3 dirLightDir = directionalLight.direction.xyz;
        vec3 dirLightColor = directionalLight.color.xyz;
        vec3 L = normalize(-dirLightDir);
        vec3 H = normalize(V + L);
        //Calculate the Aprocqimation to the fresnel equation
        vec3 F0 = mix(F0, albedo, metallic);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);  
        //calculate the cook-torrance brdf  
        float NDF = DistributionGGX(normal, H, roughness);       
        float G = GeometrySmith(normal, V, L, roughness);
 
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0)  + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(normal, L), 0.0);
        float shadow = 0;            
        if(shadowMapTarget && shadowMapping)
            shadow = CalculateShadow(fs_in.fragPosLightSpace,normal,dirLightDir);

        vec3 Lo =(1-shadow)* (kD * albedo / PI + specular) * dirLightColor * NdotL;  
        return Lo;
}

vec3 CalculatePointLight(PointLight pointLight,vec3 V,vec3 albedo,vec3 normal,float metallic,float roughness){
        vec3 pointLightPos = pointLight.position.xyz;
        vec3 pointLightColor = pointLight.color.xyz;
        vec3 L = normalize(pointLightPos -  fs_in.fragPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLightPos -  fs_in.fragPos);
        float attenuation = 1.0 / (distance);
        vec3 radiance = pointLightColor * attenuation;
        //Calculate the Aprocqimation to the fresnel equation
        vec3 F0 = mix(F0, albedo, metallic);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);  
        //calculate the cook-torrance brdf  
        float NDF = DistributionGGX(normal, H, roughness);       
        float G = GeometrySmith(normal, V, L, roughness);
 
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0)  + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(normal, L), 0.0);
        //add to the Lo           
        vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;  
        return Lo;
}


//Cook-Torrance brdf functions
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
