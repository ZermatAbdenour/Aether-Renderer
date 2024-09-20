#pragma once
#include "Scene.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
class Renderer{
public:
	Renderer()= default;
	~Renderer() = default;
	void SetupScene(Scene* scene);
	void RenderScene(Scene* scene);
	virtual GLFWwindow* Init() = 0;
	virtual void Setup() = 0;
	virtual void SetupEntity(std::shared_ptr<Entity> entity) = 0;
	virtual void SetupFrame() = 0;
	virtual void RenderEntity(std::shared_ptr<Entity> entity,Scene::Camera camera) = 0;
	virtual void Clear() = 0;
	int WindowWidth{ 800 }, WindowHeight{ 600 };
};