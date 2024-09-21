#version 330 core

in vec2 TexCoord;
in vec3 norm;
uniform sampler2D ourTexture;

void main()
{
    gl_FragColor = texture2D(ourTexture,TexCoord);
}