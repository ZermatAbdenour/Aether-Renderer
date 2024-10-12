#include "Entity.h"
#include "Ressources.h"
#include "../Utilities/FileUtil.hpp"
#include "RendererSettings.h"
#include <stb/stb_image.h>

Mesh* Ressources::Primitives::Quad = new Mesh(
	std::vector<Mesh::Vertex> {
	Mesh::Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0, 0, 1), glm::vec2(0.0f, 0.0f), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0, 0, 1), glm::vec2(0.0f, 1.0f), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0, 0, 1), glm::vec2(1.0f, 1.0f), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0, 0, 1), glm::vec2(1.0f, 0.0f), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0))
},
std::vector<unsigned int>{
	0, 2, 1,
	0, 3, 2
}
);

Mesh* Ressources::Primitives::Cube = new Mesh(
	std::vector<Mesh::Vertex> {
	// Front
	Mesh::Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),

	// Back
	Mesh::Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)),	
	Mesh::Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)),

	// Left
	Mesh::Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),

	// Right    
	Mesh::Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),
	Mesh::Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),

	// Bottom
	Mesh::Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)),
	Mesh::Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)),
	Mesh::Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)),
	Mesh::Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)),

	// Top
	Mesh::Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)),
	Mesh::Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)),
	Mesh::Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)),
	Mesh::Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1))
},
std::vector<unsigned int>{
	2, 1, 0, 0, 3, 2,   // Front
	4, 5, 6, 6, 7, 4,   // Back
	10, 9, 8, 8, 11, 10, // Left
	12, 13, 14, 14, 15, 12, // Right
	16, 17, 18, 18, 19, 16, // Bottom
	22, 21, 20, 20, 23, 22  // Top
}
);


Shader* Ressources::Shaders::Default = new Shader("Vertex.vert", "Fragment.frag");
Shader* Ressources::Shaders::ScreenShader = new Shader("Screen.vert", "Screen.frag");
Shader* Ressources::Shaders::Skybox = new Shader("Skybox.vert", "Skybox.frag");
Shader* Ressources::Shaders::Gaussianblur = new Shader("Screen.vert", "GaussianBlur.frag");
//Image Loading

Image* Ressources::LoadImageFromFile(std::string file, bool flip)
{
	Image* image = new Image();
	stbi_set_flip_vertically_on_load(flip);
	std::string path = GetImagePath(file);
	image->data = stbi_load(path.c_str(), &image->Width, &image->Height, &image->NRChannels, 0);
	image->gammaCorrect = DefaultRendererSettings.gammaCorrection;
	if (!image->data) {
		std::cout << "Failed to load Image from file :" << GetImagePath(file) << std::endl;
	}
	return image;
}

Image* Ressources::LoadImageFromPath(std::string path, bool flip)
{
	Image* image = new Image();
	stbi_set_flip_vertically_on_load(flip);
	image->data = stbi_load(path.c_str(), &image->Width, &image->Height, &image->NRChannels, 0);
	image->gammaCorrect = DefaultRendererSettings.gammaCorrection;
	if (!image->data)
		std::cout << "Failed to load Image from path :" << path<< std::endl;
	return image;
}

//Model Loading
std::shared_ptr<Entity> Ressources::LoadModelFromFile(std::string file)
{
	Assimp::Importer importer;
	std::string modelPath = GetModelPath(file);
	const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	ModelLoadingData* loadingData = new ModelLoadingData();
	loadingData->directory = modelPath.substr(0, modelPath.find_last_of('/'));
	loadingData->fileExtension = GetFileExtension(modelPath);
	loadingData->scene = scene;

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	//process root entity
	//Create the root entity
	auto rootEntity = std::make_shared<Entity>(false);

	rootEntity->Name = scene->mRootNode->mName.C_Str();

	if (scene->mRootNode->mNumMeshes > 0) {
		rootEntity->CreateMeshRenderer();
		aiMesh* mesh = scene->mMeshes[scene->mRootNode->mMeshes[0]];
		rootEntity->meshRenderer = ProcessMeshRenderer(mesh, loadingData);
	}

	for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; i++)
	{
		ProcessNode(scene->mRootNode->mChildren[i], rootEntity, loadingData);
	}
	auto end = clock();

	delete loadingData;
	return rootEntity;

}

void Ressources::ProcessNode(aiNode* node, std::shared_ptr<Entity> parent, ModelLoadingData* loadingData)
{
	//Create the root entity
	auto newEntity = std::make_shared<Entity>(false);
	newEntity->Name = node->mName.C_Str();

	if (node->mNumMeshes > 0) {
		aiMesh* mesh = loadingData->scene->mMeshes[node->mMeshes[0]];
		newEntity->meshRenderer = ProcessMeshRenderer(mesh, loadingData);
	}
	parent->AddChild(newEntity);

	//process Child 
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], newEntity, loadingData);
	}
}

MeshRenderer* Ressources::ProcessMeshRenderer(aiMesh* mesh, ModelLoadingData* loadingData)
{
	MeshRenderer* meshRenderer = new MeshRenderer();

	std::vector<Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Image*> Images;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Mesh::Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.position = position;
		
		if (mesh->mNormals) {
			glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			vertex.normal = normal;
		}

		if (mesh->mTextureCoords)
		{
			glm::vec2 textCoord;
			textCoord.x = mesh->mTextureCoords[0][i].x;
			textCoord.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = textCoord;
		}
		else
			vertex.uv = glm::vec2(0.0f, 0.0f);
		if (mesh->mTangents)
			vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

		if (mesh->mBitangents)
			vertex.biTangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
		vertices.push_back(vertex);
	}
	// process indices

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	meshRenderer->mesh = new Mesh(vertices, indices);

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = loadingData->scene->mMaterials[mesh->mMaterialIndex];

		aiString str;
		if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
			material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
			std::string fullPath = loadingData->directory + "/" + str.C_Str();

			if (!loadingData->loadedImages.contains(str.C_Str())) {
				bool flip = true;
				if (loadingData->fileExtension == "gltf")
					flip = false;

				Image* diffuseMap = LoadImageFromPath(fullPath, flip);
				meshRenderer->diffuse = diffuseMap;
				loadingData->loadedImages.insert({ str.C_Str() ,diffuseMap });
			}
			else {
				meshRenderer->diffuse = loadingData->loadedImages[str.C_Str()];
			}
		}

		//normal map
		if (material->GetTextureCount(aiTextureType_NORMALS)) {
			material->GetTexture(aiTextureType_NORMALS, 0, &str);
			std::string fullPath = loadingData->directory + "/" + str.C_Str();

			if (!loadingData->loadedImages.contains(str.C_Str())) {
				bool flip = true;
				if (loadingData->fileExtension == "gltf")
					flip = false;

				Image* normalMap = LoadImageFromPath(fullPath, flip);
				normalMap->gammaCorrect = false;
				normalMap->imageType = Image::ImageType::normal;
				meshRenderer->normalMap = normalMap;
				loadingData->loadedImages.insert({ str.C_Str() ,normalMap });
			}
			else {
				meshRenderer->normalMap = loadingData->loadedImages[str.C_Str()];
			}
		}

		//specular map
		if (material->GetTextureCount(aiTextureType_SPECULAR)) {
			material->GetTexture(aiTextureType_SPECULAR, 0, &str);
			std::string fullPath = loadingData->directory + "/" + str.C_Str();

			if (!loadingData->loadedImages.contains(str.C_Str())) {
				bool flip = true;
				if (loadingData->fileExtension == "gltf")
					flip = false;

				Image* specularMap = LoadImageFromPath(fullPath, flip);
				specularMap->gammaCorrect = false;
				meshRenderer->specularMap = specularMap;
				loadingData->loadedImages.insert({ str.C_Str() ,specularMap });
			}
			else {
				meshRenderer->specularMap = loadingData->loadedImages[str.C_Str()];
			}
		}
	}
	
	return meshRenderer;
}
