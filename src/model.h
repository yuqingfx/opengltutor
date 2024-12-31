#pragma once

#ifndef MODEL_H
#define MODEL_H

#include "shader.h"
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
	Model(char* path)
	{
		loadModel(path);
	}

	void Draw(Shader& shader);
public:
	// model data
	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;
	std::string directory; // for later use

private:
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* Mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory);
	
};


#endif // !MODEL_H
