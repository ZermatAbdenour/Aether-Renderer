#pragma once
#include "../Engine/Renderer.h"
#include "../Engine/Shader.h"
#include "../Engine/Image.h"
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
	std::unordered_map<Image*,GLuint> Textures;

	//Shaders
	GLuint PBRShader;
public:
	GLFWwindow* Init() override;
	void Setup() override;
	void SetupEntity(std::shared_ptr<Entity> entity) override;
	void SetupFrame() override;
	void RenderEntity(std::shared_ptr<Entity> entity,Scene::Camera camera) override;
	void Clear() override;
	
	//Shaders
	GLuint CreateShader(Shader* shader);

	//Meshs
	void CreateMesh(Mesh* mesh);
	std::shared_ptr<OpenglRenderer::GLMesh> GetGLMesh(Mesh* mesh);
	//Textures
	void CreateTexture(Image* image);
	GLuint GetTexture(Image* image);
};