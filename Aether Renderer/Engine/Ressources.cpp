#include "Entity.h"
#include "Ressources.h"
#include "../Utilities/FileUtil.hpp"
#include "RendererSettings.h"
#include <stb/stb_image.h>s

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

Mesh* createPlaneMesh(float width, float height, unsigned int widthSegments, unsigned int heightSegments) {
	std::vector<Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;

	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;
	float stepX = width / widthSegments;
	float stepY = height / heightSegments;

	// Create vertices
	for (unsigned int y = 0; y <= heightSegments; ++y) {
		for (unsigned int x = 0; x <= widthSegments; ++x) {
			float posX = -halfWidth + x * stepX;
			float posY = -halfHeight + y * stepY;

			Mesh::Vertex vertex;
			vertex.position = glm::vec3(posX, 0.0f, posY); // Plane on the XZ plane
			vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);    // Upwards normal
			vertex.uv = glm::vec2((float)x / widthSegments, (float)y / heightSegments);
			vertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);   // Tangent pointing in X direction
			vertex.biTangent = glm::vec3(0.0f, 0.0f, 1.0f); // BiTangent pointing in Z direction

			vertices.push_back(vertex);
		}
	}

	// Create indices
	for (unsigned int y = 0; y < heightSegments; ++y) {
		for (unsigned int x = 0; x < widthSegments; ++x) {
			unsigned int topLeft = y * (widthSegments + 1) + x;
			unsigned int topRight = topLeft + 1;
			unsigned int bottomLeft = (y + 1) * (widthSegments + 1) + x;
			unsigned int bottomRight = bottomLeft + 1;

			// First triangle
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);

			// Second triangle
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}

	return new Mesh(vertices, indices);
}


Mesh* Ressources::Primitives::Plane = createPlaneMesh(10, 10, 10, 10);

Shader* Ressources::Shaders::Default = new Shader("Vertex.vert", "Fragment.frag");
Shader* Ressources::Shaders::PBR = new Shader("vertex.vert", "PBR.frag");
Shader* Ressources::Shaders::ScreenShader = new Shader("Screen.vert", "Screen.frag");
Shader* Ressources::Shaders::Gaussianblur = new Shader("Screen.vert", "GaussianBlur.frag");
Shader* Ressources::Shaders::Kernel = new Shader("Screen.vert", "Kernel.frag");
Shader* Ressources::Shaders::EarlyDepthTesting = new Shader("EarlyDepthTesting.vert", "EarlyDepthTesting.frag");
Shader* Ressources::Shaders::SSAO = new Shader("Screen.vert", "ssao.frag");
Shader* Ressources::Shaders::SSAOBlur = new Shader("Screen.vert", "ssaoBlur.frag");
Shader* Ressources::Shaders::ShadowMap = new Shader("ShadowMap.vert", "ShadowMap.frag");
Shader* Ressources::Shaders::Skybox = new Shader("Skybox.vert", "Skybox.frag");
Shader* Ressources::Shaders::EquiRecToCubeMap = new Shader("EquiRecToCubeMap.vert", "EquiRecToCubeMap.frag");
Shader* Ressources::Shaders::DiffuseIrradiance = new Shader("EquiRecToCubeMap.vert", "DiffuseIrradiance.frag");
Shader* Ressources::Shaders::HDRPrefiltering = new Shader("EquiRecToCubeMap.vert", "HDRPrefiltering.frag");
Shader* Ressources::Shaders::ConvoluteBRDF = new Shader("ConvoluteBRDF.vert", "ConvoluteBRDF.frag");

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

	if (node->mTransformation != aiMatrix4x4()) {
		ai_real x = node->mTransformation.a4;
		ai_real y = node->mTransformation.b4;
		ai_real z = node->mTransformation.c4;
		newEntity->localPosition = glm::vec3(x,y,z);
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
				normalMap->imageType = Image::ImageType::map;
				meshRenderer->normalMap = normalMap;
				loadingData->loadedImages.insert({ str.C_Str() ,normalMap });
			}
			else {
				meshRenderer->normalMap = loadingData->loadedImages[str.C_Str()];
			}
		}
		//for (int i = 0;i < 18;i++) {
		//	int count = material->GetTextureCount(static_cast<aiTextureType>(i));
		//	if( count >0)
		//		std::cout<<i<<std::endl;
		//}
		//specular map
		if (material->GetTextureCount(aiTextureType_METALNESS)) {
			material->GetTexture(aiTextureType_METALNESS, 0, &str);
			std::string fullPath = loadingData->directory + "/" + str.C_Str();
			if (!loadingData->loadedImages.contains(str.C_Str())) {
				bool flip = true;
				if (loadingData->fileExtension == "gltf")
					flip = false;

				Image* metalicMap = LoadImageFromPath(fullPath, flip);
				metalicMap->gammaCorrect = false;
				metalicMap->imageType = Image::ImageType::map;
				meshRenderer->metalicMap = metalicMap;
				loadingData->loadedImages.insert({ str.C_Str() ,metalicMap });
			}
			else {
				meshRenderer->metalicMap = loadingData->loadedImages[str.C_Str()];
			}
		}
	}
	
	return meshRenderer;
}
