#version 460 core

const float PI = 3.14159265359;

uniform sampler2D ourTexture;

uniform float far;
uniform float near;

in VS_OUT{
    vec2 uv;
    vec3 normal;
    vec3 camPos;
} fs_in;

uniform material{
    vec3  albedo;
    float metallic;
    float roughness;
    float ao;
};
in vec3 worldPos;
out vec4 fragColor;

vec3[] lightPositions = {
    vec3(1,1,1),
    vec3(1,1,1),
    vec3(1,1,1),
    vec3(1,1,1),
};
vec3[] lightColors = {
    vec3(1,1,1),
    vec3(1,1,1),
    vec3(1,1,1),
    vec3(1,1,1),
};


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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

float GeometrySchlickGGX(float NdotV, float roughness)
{
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

void main()
{   
    vec3 N = normalize(fs_in.normal);
    vec3 V = normalize(fs_in.camPos - worldPos);
    fragColor = vec4(fs_in.camPos,1);

    vec3 Lo = vec3(0);
    //    fragColor = texture(ourTexture,fs_in.uv) * (1 - depthVec4) + depthVec4;

    for(int i = 0;i<lightPositions.length();i++){
        vec3 L = normalize(lightPositions[i] - worldPos);
        vec3 H = normalize(V + L);
        float distance    = length(lightPositions[i] - worldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = lightColors[i] * attenuation;    
    }
}