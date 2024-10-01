#pragma once
#include "../Engine/Renderer.h"
#include "../Engine/Shader.h"
#include "../Engine/Image.h"
#include <unordered_map>
#include <vector>
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
		int width, height;
		int samples;
	};
public:
	std::shared_ptr<GLMesh> screenQuad;
	std::shared_ptr<GLFrameBuffer> screenFBO;
	//maps so it does not pass the same data to the GPU if it detects that the data exist
	std::unordered_map<Mesh*,std::shared_ptr<GLMesh>> Meshs;
	std::shared_ptr<GLMesh> SkyboxMesh;
	GLuint screenTexture;
	std::unordered_map<Image*,GLuint> Textures;

	//Shaders
	GLuint screenShader;
	GLuint PBRShader;
	GLuint SkyBoxShader;

	//maps
	GLuint SkyBoxMap;
	//Uniform buffer objects
	GLuint matricesUBO;
public:
	GLFWwindow* Init() override;
	void Setup() override;
	void SetupScene(Scene* scene) override;
	void SetupEntity(std::shared_ptr<Entity> entity) override;
	void SetupFrame() override;
	void RenderEntity(MeshRenderer* meshRenderer,glm::mat4 model,Camera camera) override;
	void EndFrame() override;
	void Clear() override;
	void FrameBufferResizeCallBack(int width,int height);
	
	//Shaders
	GLuint CreateShader(Shader* shader);

	std::shared_ptr<GLFrameBuffer> CreateFrameBuffer(bool useDepthStencil,int samples);
	void UpdateFrameBuffer(std::shared_ptr<OpenglRenderer::GLFrameBuffer> frameBuffer, int width, int height);
	//Meshs
	std::shared_ptr<GLMesh> CreateMesh(Mesh* mesh);
	std::shared_ptr<GLMesh> GetGLMesh(Mesh* mesh);
	//Textures
	GLuint CreateTexture(GLenum type);
	GLuint CreateTexture(GLenum type, GLenum minFilter,GLenum magFilter);
	void SetTextureData(GLenum target, Image* image);
	void SetMultiSampleTextureData(GLenum target, Image* image,int samples);
	GLuint GetTexture(Image* image);
	GLuint CreateCubeMap(std::vector<std::string> faces);
};