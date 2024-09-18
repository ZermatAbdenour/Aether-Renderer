#pragma once
#include "Renderer.h"
#include "Scene.h"

class AeEngine {
public:
	AeEngine(Renderer* renderer);
	void Load(Scene* scene);
	~AeEngine();
private:
	Renderer* m_renderer;
	GLFWwindow* m_window;
};