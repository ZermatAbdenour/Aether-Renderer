#pragma once
#include "Renderer.h"
#include "Window.h"
#include "Scene.h"

class AeEngine {
public:
	AeEngine();
	void Load(Scene* scene);
	~AeEngine();
private:
	Renderer* m_renderer;
	Window* m_window;
};