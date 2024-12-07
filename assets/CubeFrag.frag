#version 330 core

struct Material
{
	sampler2D diffuse;
	vec3 specular;
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

void main()
{
	
	vec3 ambient = light.ambient * vec3(texture(mat.diffuse, TexCoord));
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position- FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, TexCoord));

	
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
	vec3 specular = mat.specular * spec * light.specular;

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, 1.0);

} 