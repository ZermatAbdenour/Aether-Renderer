#pragma once
#include <glm/glm.hpp>
#include "Scene.h"
#include "RendererSettings.h"
class Editor;
class Renderer{
public:
	Renderer()= default;
	~Renderer() = default;
	void Render(Scene* scene);
	
	//renderer loop
	virtual GLFWwindow* Init() = 0;
	virtual void Setup(Scene* scene) = 0;
	virtual void SetupEntity(std::shared_ptr<Entity> entity) = 0;
	virtual void RenderScene(Scene* scene) = 0;
	virtual void RenderEditor(Editor* editor) = 0;
	virtual void PostProcess() = 0;
	virtual void Clear() = 0;

	//UI
	virtual void LoadSkyBox(Image* image) = 0;
	virtual intptr_t GetUITexture(Image* image) = 0;
	virtual intptr_t GetShadowMapTexture() = 0;
	virtual intptr_t GetSkyBox() = 0;
	virtual void RendererSettingsTab() = 0;
	

	int windowWidth{ 1000 }, windowHeight{ 800 };
	RendererSettings settings;
};