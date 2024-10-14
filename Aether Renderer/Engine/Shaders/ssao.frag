#version 460 core

in vec2 TexCoord;

uniform sampler2D depthTexture;

uniform sampler2D noiseTexture;
// visibility radius
uniform float sampleRad;
layout(std430, binding = 0) buffer SSAOKernel {
    vec3 kernel[];
};
uniform vec2 noiseScale;

uniform mat4 projection;
uniform mat4 projectionInv;
uniform float power = 2; 
out float fragColor;
vec3 calcViewPosition(vec2 coords){
    float depth = texture(depthTexture, coords).r;

    float z = depth * 2.0 - 1.0;
    vec4 clipSpacePosition = vec4(TexCoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = projectionInv * clipSpacePosition;
    //vec4 worldSpacePosition = viewMatrixInv * viewSpacePosition;
    return viewSpacePosition.xyz/viewSpacePosition.w;
}
void main(){
    vec3 viewPos = calcViewPosition(TexCoord);

    vec3 viewNormal = cross(dFdy(viewPos.xyz), dFdx(viewPos.xyz));
    viewNormal = normalize(viewNormal * -1.0);
    vec3 randomVec = texture(noiseTexture, TexCoord * noiseScale).xyz;
    vec3 tangent = normalize(randomVec - viewNormal * dot(randomVec, viewNormal));
    vec3 bitangent = cross(viewNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, viewNormal);

    float occlusion_factor = 0.0;
    for (int i = 0 ; i < kernel.length() ; i++) {
            vec3 samplePos = TBN * kernel[i];

            // here we calculate the sampling point position in view space.
            samplePos = viewPos + samplePos * sampleRad;

            // now using the sampling point offset
            vec4 offset = vec4(samplePos, 1.0);
            offset = projection * offset;
            offset.xy /= offset.w;
            offset.xy = offset.xy * vec2(0.5) + vec2(0.5);

            // this is the geometry's depth i.e. the view_space_geometry_depth
            // this value is negative in my coordinate system
            float geometryDepth = calcViewPosition(offset.xy).z;
            
            float rangeCheck = smoothstep(0.0, 1.0, sampleRad / abs(viewPos.z - geometryDepth));
            
            // samplePos.z is the sample's depth i.e. the view_space_sampling_position depth
            // this value is negative in my coordinate system
            // for occlusion to be true the geometry's depth should be greater or equal (equal or less negative and consequently closer to the camera) than the sample's depth

            occlusion_factor += (geometryDepth >= samplePos.z  ? 1.0 : 0.0) * rangeCheck; 
        }
        // we will devide the accmulated occlusion by the number of samples to get the average occlusion value. 
        float average_occlusion_factor = occlusion_factor * 1/kernel.length();
        
        float visibility_factor = 1.0 - average_occlusion_factor;

        // We can raise the visibility factor to a power to make the transition
        // more sharp. Experiment with the value of this power to see what works best for you.
        // Even after raising visibility to a power > 1, the range still remains between [0.0, 1.0].
        fragColor = pow(visibility_factor, power);

}

