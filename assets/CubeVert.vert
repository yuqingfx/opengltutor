#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 LightingColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{

	gl_Position = proj * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	vec3 Normal = mat3(transpose(inverse(view * model))) *  aNormal;
	vec3 FragPos = vec3(view * model * vec4(aPos, 1.0));
	vec3 LightPos = vec3(view * model * vec4(lightPos, 1.0));
	vec3 ViewPos = vec3(view * vec4(viewPos, 1.0));

	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specularStrength = .5;
	vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	LightingColor = (ambient + diffuse + specular) * objectColor;

}