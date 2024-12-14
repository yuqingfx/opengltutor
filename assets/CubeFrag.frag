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

	float k_constant;
	float k_linear;
	float k_quadratic;

	float cutoff;
	float outercutoff;
};


#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirectionalLight dirLight;
uniform SpotLight spotLight;
uniform Material mat;

uniform vec3 viewPos;
uniform float time;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

// light related functions prototype
vec3 CalcDirLight(DirectionalLight Light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir); 

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	

	// emission
	// float ds = 0.125;
	// vec3 emission = texture(mat.emission, TexCoord * (1.0 + ds * 2) - ds).rgb;
	// emission = emission * (sin(time) * 0.5 + 0.5) * 2.0;
	
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	for(int i=0; i< NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}

	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

	FragColor = vec4(result, 1.0);
} 


vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);


	vec3 ambient = light.ambient * vec3(texture(mat.diffuse, TexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, TexCoord));
	vec3 specular = light.specular * spec * vec3(texture(mat.specular, TexCoord));

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
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

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);

	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.k_constant + light.k_linear * dist + light.k_quadratic * pow(dist, 2));

	float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.outercutoff;
    float intensity = clamp((theta - light.outercutoff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * vec3(texture(mat.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(mat.specular, TexCoord));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}





