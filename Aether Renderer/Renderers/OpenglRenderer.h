#pragma once
#include "../Engine/Renderer.h"
#include "../Engine/Shader.h"
#include <map>

class OpenglRenderer:public Renderer
{
public:
	struct GLMesh {
		GLuint vbo;
		GLuint ebo;
		GLuint vao;
	};
public:
	//maps so it does not pass the same data to the GPU if it detects that the data exist
	std::unordered_map<Mesh*,std::shared_ptr<GLMesh>> Meshs;
	std::unordered_map<Shader*,GLuint> Shaders;
public:
	GLFWwindow* Init() override;
	void Setup() override;
	void SetupEntity(std::shared_ptr<Entity> entity) override;
	void SetupFrame() override;
	void RenderEntity(std::shared_ptr<Entity> entity,Scene::Camera camera) override;
	void Clear() override;
	
	//Shaders
	void CreateShader(Shader* shader);

	//Mesh
	void CreateMesh(Mesh* mesh);
};