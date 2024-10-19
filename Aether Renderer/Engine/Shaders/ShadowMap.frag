#version 460 core

out vec4 fragColor;
void main(){
   fragColor = vec4(vec3(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z),1);
}