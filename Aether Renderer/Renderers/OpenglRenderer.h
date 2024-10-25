#pragma once
#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_opengl3.h>
#include <Imgui/imgui_impl_glfw.h>
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
	enum DepthStencilType {
		None,
		RBODepth,
		RBODepthStencil,
		TextureDepth,
		TextureDepthStencil
	};
	struct GLFrameBuffer {
		GLuint id;
		std::vector<GLuint> colorAttachments;
		GLuint depthStencilBuffer;
		DepthStencilType depthStencilType;
		Image* image;
		bool HDR;
		int samples;
	};
public:
	//Lights
	static const int MAX_DIRECTIONALLIGHTS = 5;
	static const int MAX_POINTLIGHTS = 10;
	struct GLPointLight {
		glm::vec4 position;
		glm::vec4 color;
		GLPointLight(PointLight pointLight) {
			position = glm::vec4(pointLight.position,0);
			color = pointLight.color * pointLight.intensity;
		}
		GLPointLight() = default;
	};
	struct GLDirectionalLight {
		glm::vec4 direction;
		glm::vec4 color;
		GLDirectionalLight(DirectionalLight directionalLight) {
			direction = glm::vec4(directionalLight.direction,0);
			color = directionalLight.color;
			color = directionalLight.color * directionalLight.intensity;
		}
		GLDirectionalLight() = default;
	};
private:
	//FrameBuffers
	std::shared_ptr<GLFrameBuffer> m_screenFBO;
	std::shared_ptr<GLFrameBuffer> m_autoExposureFBO;
	std::shared_ptr<GLFrameBuffer> m_boomPingpongFBOs[2];
	std::shared_ptr<GLFrameBuffer> m_ssaoFBO;
	std::shared_ptr<GLFrameBuffer> m_resolveDepthFBO;
	std::shared_ptr<GLFrameBuffer> m_ssaoBlurFBO;
	std::shared_ptr<GLFrameBuffer> m_shadowDepthFBO;

	//Meshes
	//maps so it does not pass the same data to the GPU if it detects that the data exist
	std::unordered_map<Mesh*,std::shared_ptr<GLMesh>> m_meshs;
	std::shared_ptr<GLMesh> m_screenQuad;
	std::shared_ptr<GLMesh> m_cubeMesh;

	//Textures
	std::unordered_map<Image*,GLuint> m_textures;
	GLuint m_ssaoNoiseTexture;
	GLuint m_skyBoxMap;

	//Shaders
	GLuint m_screenShader;
	GLuint m_PBRShader;
	GLuint m_earlyDepthTestingShader;
	GLuint m_skyBoxShader;
	GLuint m_gaussianBlurShader;
	GLuint m_kernelBlurShader;
	GLuint m_ssaoShader;
	GLuint m_ssaoBlurShader;
	GLuint m_shadowMapShader;
	GLuint m_EquiRecToCubeMapShader;

	//Uniform buffer objects
	GLuint m_matricesUBO;
	GLuint m_lightsUBO;

	GLuint ssaoKernelSSBO;

	std::vector<glm::vec3> m_ssaokernel;
	std::vector<glm::vec3> m_ssaoNoise;
	glm::mat4 m_lightSpaceMatrix;
public:
	GLFWwindow* Init() override;
	void Setup(Scene* scene) override;
	void SetupEntity(std::shared_ptr<Entity> entity) override;
	void RenderScene(Scene* scene)override;
	void PostProcess() override;
	void RenderEditor(Editor* editor) override;
	void Clear() override;
	void FrameBufferResizeCallBack(int width,int height);
	void EarlyDepthTestEntity(MeshRenderer* meshRenderer, glm::mat4 model);
	void ShadowMapEntity(MeshRenderer* meshRenderer, glm::mat4 model);
	void RenderEntity(MeshRenderer* meshRenderer, glm::mat4 model);
	
	//Shaders
	GLuint CreateShader(Shader* shader);

	GLuint CreateComputeShader(ComputeShader* shader);

	std::shared_ptr<GLFrameBuffer> CreateFrameBuffer();
	void DeleteFrameBuffer(std::shared_ptr<GLFrameBuffer> framebuffer);
	void SetFrameBufferAttachements(std::shared_ptr<OpenglRenderer::GLFrameBuffer> framebuffer,int width,int height,int colorAttachmentsCount, bool hdr,DepthStencilType depthStencilType, int sample);
	//Meshs
	std::shared_ptr<GLMesh> CreateMesh(Mesh* mesh);
	std::shared_ptr<GLMesh> GetGLMesh(Mesh* mesh);
	//Textures
	GLuint CreateTexture(GLenum type, GLenum minFilter = NULL,GLenum magFilter = NULL,GLenum wrapS = NULL,GLenum wrapT = NULL);
	void SetTextureData(GLenum target, Image* image);
	GLuint GetTexture(Image* image);
	
	GLuint CreateHDRCubeMap(Image* image,int width,int height);
	GLuint CreateCubeMap(std::vector<std::string> faces);

	//UI
	intptr_t GetUITexture(Image* image) override;
	intptr_t GetShadowMapTexture() override;
	void RendererSettingsTab() override;
	void ReloadTextures(bool gammaCorrection);
};