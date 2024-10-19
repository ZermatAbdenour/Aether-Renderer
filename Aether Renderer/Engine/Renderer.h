#pragma once
#include <glm/glm.hpp>
#include "Scene.h"
#include "RendererSettings.h"

class Renderer{
public:
	Renderer()= default;
	~Renderer() = default;
	virtual void SetupScene(Scene* scene);
	void RenderScene();
	
	//renderer loop
	virtual GLFWwindow* Init() = 0;
	virtual void RenderFrame() = 0;
	virtual void SetupEntity(std::shared_ptr<Entity> entity) = 0;
	virtual void SetupFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void Clear() = 0;

	//UI
	virtual intptr_t GetUITexture(Image* image) = 0;
	virtual intptr_t GetShadowMapTexture() = 0;
	virtual void RendererSettingsTab() = 0;
	

	int windowWidth{ 1000 }, windowHeight{ 800 };
	RendererSettings settings;
protected:
	Scene* m_currentScene;
};