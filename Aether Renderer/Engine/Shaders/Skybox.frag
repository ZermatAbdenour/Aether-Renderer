#version 460 core

uniform samplerCube cubeMap;

in vec3 texCoord;

out vec4 fragColor;
out vec4 bloomColor;
void main(){
    fragColor = texture(cubeMap,texCoord);
    
    //Calculate bloom color
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bloomColor = vec4(fragColor.rgb, 1.0);
    else
        bloomColor = vec4(0.0, 0.0, 0.0, 1.0);
}