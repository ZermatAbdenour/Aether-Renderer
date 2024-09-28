#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <unordered_map>

class Entity;
class Image;
class MeshRenderer;

class Ressources {
public:
	class Primitives {
	public:
		static Mesh* Quad;
		static Mesh* Cube;
	};
	
	class Shaders {
	public:
		static Shader* Default;
		static Shader* ScreenShader;
		static Shader* Skybox;
	};
	/// <summary>
	/// Load Immage from a file in the Resources/Images folder
	/// </summary>
	/// <param name="file"></param>
	/// <param name="flip"> flips the Image on load</param>
	/// <returns></returns>
	static Image* LoadImageFromFile(std::string file, bool flip = false);
	/// <summary>
	/// Load Image from full path
	/// </summary>
	/// <param name="file"></param>
	/// <param name="flip"></param>
	/// <returns></returns>
	static Image* LoadImageFromPath(std::string path, bool flip = false);
	//TODO:Test Model loading for Multiple format file works with : OBJ
	/// <summary>
	/// Load a 3d model from a file 
	/// </summary>
	/// <param name="path"></param>
	/// <returns>Root entity</returns>
	static std::shared_ptr<Entity> LoadModelFromFile(std::string path);
private:
	struct ModelLoadingData {
		const aiScene* scene;
		std::string directory;
		std::string fileExtension;
		std::unordered_map<std::string,Image*> loadedImages;
	};
	static void ProcessNode(aiNode* node, std::shared_ptr<Entity> parent, ModelLoadingData* loadingData);
	static MeshRenderer* ProcessMeshRenderer(aiMesh* mesh, ModelLoadingData* loadingData);
};