#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Scene.h"

class Renderer{
public:
	Renderer()= default;
	~Renderer() = default;
	virtual void SetupScene(Scene* scene);
	void RenderScene();
	virtual GLFWwindow* Init() = 0;
	virtual void Setup() = 0;
	virtual void SetupEntity(std::shared_ptr<Entity> entity) = 0;
	virtual void SetupFrame() = 0;
	virtual void RenderEntity(MeshRenderer* meshRenderer, glm::mat4 model,Camera camera) = 0;
	virtual void EndFrame() = 0;
	virtual void Clear() = 0;
	int windowWidth{ 1000 }, windowHeight{ 800 };

protected:
	Scene* m_currentScene;
};