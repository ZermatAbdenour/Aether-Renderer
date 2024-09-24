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
	struct GLFrameBuffer {
		GLuint id;
		GLuint colorAttachment;
		GLuint depthStencilRBO;
	};
public:
	std::shared_ptr<GLMesh> screenQuad;
	std::shared_ptr<GLFrameBuffer> FBO;
	//maps so it does not pass the same data to the GPU if it detects that the data exist
	std::unordered_map<Mesh*,std::shared_ptr<GLMesh>> Meshs;
	std::unordered_map<Image*,GLuint> Textures;

	//Shaders
	GLuint screenShader;
	GLuint PBRShader;
public:
	GLFWwindow* Init() override;
	void Setup() override;
	void SetupEntity(std::shared_ptr<Entity> entity) override;
	void SetupFrame() override;
	void RenderEntity(MeshRenderer* meshRenderer,glm::mat4 model,Camera camera) override;
	void EndFrame() override;
	void Clear() override;
	
	//Shaders
	GLuint CreateShader(Shader* shader);

	std::shared_ptr<GLFrameBuffer> CreateFramebuffer();
	//Meshs
	std::shared_ptr<GLMesh> CreateMesh(Mesh* mesh);
	std::shared_ptr<GLMesh> GetGLMesh(Mesh* mesh);
	//Textures
	GLuint CreateTexture(Image* image);
	GLuint GetTexture(Image* image);
};