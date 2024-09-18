#pragma once
#include "Scene.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
class Renderer{
public:
	Renderer()= default;
	~Renderer() = default;
	void Render(Scene* scene);
	virtual GLFWwindow* Init() = 0;
	virtual void Setup() = 0;
	virtual void FrameSetup() = 0;
	virtual void RenderEntity(std::shared_ptr<Entity> entity) = 0;
	virtual void Clear() = 0;
	int WindowWidth{ 800 }, WindowHeight{ 600 };
};