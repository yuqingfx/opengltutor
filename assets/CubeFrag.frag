#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;

};

struct Light
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

uniform Light light;
uniform Material mat;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;

void main()
{
	// ambient
	vec3 ambient = light.ambient * vec3(texture(mat.diffuse, TexCoord));
	
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position- FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, TexCoord));

	// spec
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
	vec3 specular = light.specular * spec * vec3(texture(mat.specular, TexCoord));

	// emission
	
	float ds = 0.125;
	vec3 emission = texture(mat.emission, TexCoord * (1.0 + ds * 2) - ds).rgb;
	emission = emission * (sin(time) * 0.5 + 0.5) * 2.0;
	vec3 result = ambient + diffuse + specular + emission;
	FragColor = vec4(result, 1.0);
} 