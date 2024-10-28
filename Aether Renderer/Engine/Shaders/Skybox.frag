#version 460 core

uniform samplerCube cubeMap;

in vec3 texCoord;
in vec3 localPos;

out vec4 fragColor;
out vec4 bloomColor;
void main(){
    //vec3 envColor = texture(cubeMap, texCoord).rgb;
    vec3 envColor = textureLod(cubeMap, texCoord,1.2).rgb;
    
    fragColor = vec4(envColor, 1.0);

    //Calculate bloom color
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bloomColor = vec4(fragColor.rgb, 1.0);
    else
        bloomColor = vec4(0.0, 0.0, 0.0, 1.0);
}