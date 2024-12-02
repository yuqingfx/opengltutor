#version 330 core
out vec4 FragColor;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	// FragColor = mix(texture(InTexture, TexCoord), texture(ourTexture2, TexCoord), 0.9);

	float ambientStrength = 0.9f;
	vec3 ambient = ambientStrength * lightColor;

	vec3 result = ambient * objectColor;

	FragColor = vec4(result, 1.0);

} 