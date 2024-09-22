#include "Entity.h"
#include "Ressources.h"
#include "../Utilities/FileUtil.hpp"

#include <stb/stb_image.h>

Mesh* Ressources::Primitives::Quad = new Mesh(
std::vector<Mesh::Vertex> {
	Mesh::Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0, 0, 1), glm::vec2(0.0f, 0.0f)),
	Mesh::Vertex(glm::vec3(-0.5f, 0.5f, 0.0f) , glm::vec3(0, 0, 1), glm::vec2(0.0f, 1.0f)),
	Mesh::Vertex(glm::vec3(0.5f, 0.5f, 0.0f)  , glm::vec3(0, 0, 1), glm::vec2(1.0f, 1.0f)),
	Mesh::Vertex(glm::vec3(0.5f, -0.5f, 0.0f) , glm::vec3(0, 0, 1), glm::vec2(1.0f, 0.0f))

},
std::vector<unsigned int>{
	0, 2, 1,
	0, 3, 2
}
);

Shader* Ressources::Shaders::Default = new Shader("VertexShader.vert", "FragmentShader.frag");

//Image Loading

Image* Ressources::LoadImageFromFile(std::string file, bool flip)
{
	Image* image = new Image();
	stbi_set_flip_vertically_on_load(flip);

	image->data = stbi_load(GetImagePath(file).c_str(), &image->Width, &image->Height, &image->NRChannels, 0);

	if (!image->data)
		std::cout << "Failed to load Image" << std::endl;
	return image;
}

Image* Ressources::LoadImageFromPath(std::string path, bool flip)
{
	Image* image = new Image();
	stbi_set_flip_vertically_on_load(flip);
	image->data = stbi_load(path.c_str(), &image->Width, &image->Height, &image->NRChannels, 0);
	if (!image->data)
		std::cout << "Failed to load Image" << std::endl;
	return image;
}

//Model Loading
std::shared_ptr<Entity> Ressources::LoadModelFromFile(std::string file)
{
	Assimp::Importer importer;
	std::string modelDirectory = GetModelPath(file);
	const aiScene* scene = importer.ReadFile(modelDirectory, aiProcess_Triangulate | aiProcess_FlipUVs);
	ModelLoadingData* loadingData = new ModelLoadingData();
	loadingData->directory = modelDirectory.substr(0, modelDirectory.find_last_of('/'));;
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
		
		glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		vertex.normal = normal;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 textCoord;
			textCoord.x = mesh->mTextureCoords[0][i].x;
			textCoord.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = textCoord;
		}
		else
			vertex.uv = glm::vec2(0.0f, 0.0f);

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
		material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		std::string fullPath = loadingData->directory + "/" + str.C_Str();

		if (!loadingData->loadedImages.contains(str.C_Str())) {
			Image* diffuseMap = LoadImageFromPath(fullPath,true);
			meshRenderer->image = diffuseMap;
			loadingData->loadedImages.insert({ str.C_Str() ,diffuseMap });
		}
		else {
			meshRenderer->image = loadingData->loadedImages[str.C_Str()];
		}
	}
	
	return meshRenderer;
}
