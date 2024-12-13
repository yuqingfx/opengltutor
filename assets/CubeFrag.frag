#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;

};

struct DirectionalLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction; 
};

struct PointLight
{
	vec3 position; 

	float k_constant;
	float k_linear;
	float k_quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position; 
	vec3 direction;
	
	float cutoff;
	float outercutoff;
};

vec3 CalcDirLight(DirectionalLight dirLight, vec3 normal, vec3 viewDir, Material mat, vec2 TexCoord)
{
	vec3 lightDir = normalize(-dirLight.direction);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);

	vec3 ambient = dirLight.ambient * vec3(texture(mat.diffuse, TexCoord));
	vec3 diffuse = dirLight.diffuse * diff * vec3(texture(mat.diffuse, TexCoord));
	vec3 specular = dirLight.specular * spec * vec3(texture(mat.specular, TexCoord));

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material mat, vec2 TexCoord)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);

	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.k_constant + light.k_linear * dist + light.k_quadratic * pow(dist, 2));

	vec3 ambient = light.ambient * vec3(texture(mat.diffuse, TexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, TexCoord));
	vec3 specular = light.specular * spec * vec3(texture(mat.specular, TexCoord));

	return (ambient + diffuse + specular);
}




#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirectionalLight dirLight;
uniform SpotLight light;

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
	
	// diffuse
	vec3 norm = normalize(Normal);

	
	// spec
	vec3 viewDir = normalize(viewPos - FragPos);
	
	// emission
	float ds = 0.125;
	vec3 emission = texture(mat.emission, TexCoord * (1.0 + ds * 2) - ds).rgb;
	emission = emission * (sin(time) * 0.5 + 0.5) * 2.0;
	
	vec3 result = CalcDirLight(dirLight, norm, viewDir, mat, TexCoord);

	
	FragColor = vec4(result, 1.0);
} 