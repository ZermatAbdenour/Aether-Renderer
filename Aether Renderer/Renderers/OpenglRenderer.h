#pragma once
#include "../Engine/Renderer.h"
#include "../Engine/Shader.h"
#include <map>

class OpenglRenderer:public Renderer
{
public:
	struct GLShader {
		GLuint id;
	};
	struct GLMesh {
		GLuint VBO;
		GLuint EBO;
		GLuint VAO;
	};
public:
	std::map<Mesh*,std::shared_ptr<GLMesh>> Meshs;
	std::map<Shader*,std::shared_ptr<GLShader>> Shaders;
public:
	GLFWwindow* Init() override;
	void Setup(Scene* scene) override;
	void FrameSetup() override;
	void RenderEntity(std::shared_ptr<Entity> entity) override;
	void Clear() override;
	
	//Shaders
	void CreateShader(Shader* shader);

	//Mesh
	void CreateMesh(Mesh* mesh);
};