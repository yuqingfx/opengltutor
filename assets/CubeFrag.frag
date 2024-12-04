#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;

};

struct Light
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;
uniform Material mat;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	
	vec3 ambient = light.ambient * mat.ambient;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * mat.diffuse;

	
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
	vec3 specular = mat.specular * spec * light.specular;

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, 1.0);

} 