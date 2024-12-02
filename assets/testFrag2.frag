#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D InTexture;


void main()
{
    //FragColor = texture(InTexture, TexCoord);
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
} 