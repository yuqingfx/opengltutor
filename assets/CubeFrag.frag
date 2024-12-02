#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D InTexture;
uniform sampler2D ourTexture2;

void main()
{
	//FragColor = mix(texture(InTexture, TexCoord), texture(ourTexture2, TexCoord), 0.9);
	FragColor = vec4(.3, .5, .7, 1.0);
} 