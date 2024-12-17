#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include "shader.h"


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
};

class Mesh
{
public:
	// Mesh data
	std::vector<Vertex>			vertices;
	std::vector<unsigned int>	indices;
	std::vector<Texture>		textures;
	unsigned int VAO;

	Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, std::vector<Texture> Textures)
		: vertices(Vertices), indices(Indices), textures(Textures)
	{
		setupMesh();
	};

	void Draw(Shader& shader)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string number; // wha is this number
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
			{
				number = std::to_string(diffuseNr++);
			}
			else if (name == "texture_specular")
			{
				number = std::to_string(specularNr++);
			}
			else if (name == "texture_normal")
			{
				number = std::to_string(normalNr++);
			}
			else if (name == "texture_height")
			{
				number = std::to_string(heightNr++);
			}
			// what is this name + number thing?
			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		
	}


private:
	unsigned int VBO, EBO;

	void setupMesh()
	{

	};

};