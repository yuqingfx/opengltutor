#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float InMixValue;
void main()
{
    // FragColor = texture(ourTexture, TexCoord);
    FragColor = mix(texture(ourTexture, TexCoord), texture(ourTexture2, vec2(1.0 - TexCoord.x, TexCoord.y)), InMixValue);
} 