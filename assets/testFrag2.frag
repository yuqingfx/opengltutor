#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D InTexture;
// uniform sampler2D ourTexture2;

void main()
{
    FragColor = texture(InTexture, TexCoord);
    // FragColor = mix(texture(InTexture, TexCoord), texture(ourTexture2, TexCoord), 0.9);
} 